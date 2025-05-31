#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

pragma comment(lib, "ws2_32.lib")

#define PORT 12345
#define BUFFER_SIZE 256

#ifndef fileONLINE
#define fileONLINE

typedef struct {
    WSADATA wsaData;
    int serverSocket;
    int clientSocket;
    int isServer;
    int isConnected;
    char localPlayerName[100];
    char opponentName[100];
    char serverIP[INET_ADDRSTRLEN];
} Network;

Network* createNetwork() {
    Network* nw = malloc(sizeof(Network));
    if (!nw) return NULL;

    memset(nw, 0, sizeof(Network));
    nw->serverSocket = INVALID_SOCKET;
    nw->clientSocket = INVALID_SOCKET;
    return nw;
}

void destroyNetwork(Network* nw) {
    if (nw) {
        terminateOnline(nw);
        WSACleanup();
        free(nw);
    }
}

// --- //

char* getPublicIP(Network* nw) {
    char hostname[256];

    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        strcpy(nw->serverIP, "127.0.0.1");
        return nw->serverIP;
    }

    struct hostent *hp = gethostbyname(hostname);
    if (hp == NULL) {
        strcpy(nw->serverIP, "127.0.0.1");
        return nw->serverIP;
    }

    for (int i = 0; hp->h_addr_list[i] != NULL; i++) {
        struct in_addr addr;
        memcpy(&addr, hp->h_addr_list[i], sizeof(addr));
        const char *ipStr = inet_ntoa(addr);
        if (strncmp(ipStr, "127.", 4) != 0) {
            strcpy(nw->serverIP, ipStr);
            return nw->serverIP;
        }
    }

    struct in_addr addr;
    memcpy(&addr, hp->h_addr_list[0], sizeof(addr));
    strcpy(nw->serverIP, inet_ntoa(addr));
    return nw->serverIP;
}

int createServer(Network* nw, const char* playerName) {
    strcpy(nw->localPlayerName, playerName);
    nw->isServer = 1;
    return openOnline(nw);
}

int joinServer(Network* nw, const char* ip, const char* playerName) {
    // return 1 if worked, 0 if not
    strcpy(nw->localPlayerName, playerName);
    nw->isServer = 0;

    nw->clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (nw->clientSocket == INVALID_SOCKET) {
        printf("Client socket creation failed: %d\n", WSAGetLastError());
        return 0;
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, ip, &serverAddr.sin_addr) <= 0) {
        printf("Invalid IP address\n");
        closesocket(nw->clientSocket);
        nw->clientSocket = INVALID_SOCKET;
        return 0;
    }

    if (connect(nw->clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Connection failed: %d\n", WSAGetLastError());
        closesocket(nw->clientSocket);
        nw->clientSocket = INVALID_SOCKET;
        return 0;
    }

    nw->isConnected = 1;

    char nameMsg[128];
    snprintf(nameMsg, sizeof(nameMsg), "\x03n %s", playerName);
    send(nw->clientSocket, nameMsg, strlen(nameMsg), 0);

    printf("Connected to server at %s\n", ip);
    return 1;
}

//- MANAGE CONNECTION -//

int openOnline(Network* nw) {
    // return 1 if worked, 0 if not
    if (nw->isServer) {
        nw->serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (nw->serverSocket == INVALID_SOCKET) {
            printf("Server socket creation failed: %d\n", WSAGetLastError());
            return 0;
        }

        struct sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(PORT);

        if (bind(nw->serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            printf("Bind failed: %d\n", WSAGetLastError());
            closesocket(nw->serverSocket);
            nw->serverSocket = INVALID_SOCKET;
            return 0;
        }

        if (listen(nw->serverSocket, 1) == SOCKET_ERROR) {
            printf("Listen failed: %d\n", WSAGetLastError());
            closesocket(nw->serverSocket);
            nw->serverSocket = INVALID_SOCKET;
            return 0;
        }

        strcpy(nw->serverIP, getPublicIP(nw));
        printf("Server listening on %s:%d\n", nw->serverIP, PORT);

        struct sockaddr_in clientAddr;
        int clientLen = sizeof(clientAddr);
        nw->clientSocket = accept(nw->serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (nw->clientSocket == INVALID_SOCKET) {
            printf("Accept failed: %d\n", WSAGetLastError());
            closesocket(nw->serverSocket);
            nw->serverSocket = INVALID_SOCKET;
            return 0;
        }

        nw->isConnected = 1;
        printf("Client connected from %s\n", inet_ntoa(clientAddr.sin_addr));
        return 1;
    } else {
        return 1;
    }
}

int terminateOnline(Network* nw) {
    // Send request that you leave, if a connection exists
    // return 1 if closed, 0 if failed
    if (nw->isConnected) {
        char disconnectMsg[] = "\x04";
        if (nw->clientSocket != INVALID_SOCKET) {
            send(nw->clientSocket, disconnectMsg, strlen(disconnectMsg), 0);
        }
    }

    if (nw->clientSocket != INVALID_SOCKET) {
        closesocket(nw->clientSocket);
        nw->clientSocket = INVALID_SOCKET;
    }

    if (nw->serverSocket != INVALID_SOCKET) {
        closesocket(nw->serverSocket);
        nw->serverSocket = INVALID_SOCKET;
    }

    nw->isConnected = 0;
    nw->isServer = 0;
    return 1;
}


void closeOnline(Network* nw) {
    //opponant left
    if (nw->clientSocket != INVALID_SOCKET) {
        closesocket(nw->clientSocket);
        nw->clientSocket = INVALID_SOCKET;
    }
    nw->isConnected = 0;
    printf("Opponent disconnected\n");
}

//- COMMUNICATION -//

int sendChatMessage(Network* nw, const char* message) {
    if (!nw->isConnected || nw->clientSocket == INVALID_SOCKET) {
        return 0;
    }

    char filteredMsg[BUFFER_SIZE];
    int j = 0;
    for (int i = 0; message[i] != '\0' && j < sizeof(filteredMsg) - 1; i++) {
        if (message[i] >= 32 && message[i] < 127) {
            filteredMsg[j++] = message[i];
        }
    }
    filteredMsg[j] = '\0';

    if (send(nw->clientSocket, filteredMsg, strlen(filteredMsg), 0) == SOCKET_ERROR) {
        printf("Chat send failed: %d\n", WSAGetLastError());
        closeOnline(nw);
        return 0;
    }

    return 1;
}

int onlinePing(Network* nw) {
    // sends a ping to see if opponent still responds
    if (!nw->isConnected || nw->clientSocket == INVALID_SOCKET) {
        return 0;
    }

    char pingMsg[32];
    snprintf(pingMsg, sizeof(pingMsg), "\x10%s", nw->serverIP);

    if (send(nw->clientSocket, pingMsg, strlen(pingMsg), 0) == SOCKET_ERROR) {
        printf("Ping send failed: %d\n", WSAGetLastError());
        closeOnline(nw);
        return 0;
    }

    return 1;
}

int receiveMessage(Network* nw) {
    // if cmd starts with symbol before ASCII 32 that will be filtered when sending a message, its a command
    // cmd list examples: m 4725 (move), c 02 (camp), n Name (name), d (disconnect), p IP (ping)
    // else the message is a chat
    // return 1 if message interpreted, 0 if failed

    if (!nw->isConnected || nw->clientSocket == INVALID_SOCKET) {
        return 0;
    }

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    int bytesReceived = recv(nw->clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived == SOCKET_ERROR) {
        printf("Receive failed: %d\n", WSAGetLastError());
        closeOnline(nw);
        return 0;
    }

    if (bytesReceived == 0) {
        printf("Connection closed by opponent\n");
        closeOnline(nw);
        return 0;
    }

    buffer[bytesReceived] = '\0';

    if (buffer[0] < 32) {
        switch (buffer[0]) {
            case 1:
                if (strlen(buffer) > 2 && buffer[1] == 'm') {
                    int moveData;
                    if (sscanf(buffer + 3, "%d", &moveData) == 1) {
                        int from = moveData / 100;
                        int to = moveData % 100;
                        printf("Received move command: from %d to %d\n", from, to);
                    }
                }
                break;

            case 2:
                if (strlen(buffer) > 2 && buffer[1] == 'c') {
                    int pawn;
                    if (sscanf(buffer + 3, "%d", &pawn) == 1) {
                        printf("Received camp command for pawn %d\n", pawn);
                    }
                }
                break;

            case 3:
                if (strlen(buffer) > 2 && buffer[1] == 'n') {
                    strncpy(nw->opponentName, buffer + 3, sizeof(nw->opponentName) - 1);
                    nw->opponentName[sizeof(nw->opponentName) - 1] = '\0';
                    printf("Opponent name: %s\n", nw->opponentName);
                }
                break;

            case 4:
                printf("Opponent disconnected\n");
                closeOnline(nw);
                break;

            case 16:
                if (strlen(buffer) > 2 && buffer[1] == 'p') {
                    printf("Received ping from %s\n", buffer + 3);
                }
                break;

            default:
                printf("Unknown command received\n");
                return 0;
        }
    } else {
        printf("Chat message: %s\n", buffer);
    }

    return 1;
}

int onlineMovePawn(Network* nw, int from, int to) {
    // for m4725, from = 47 and to = 25
    // checks to see if the pawn can be moved here.
    // return 1 if moved, 0 if failed (will send message to opp move is invalid)
    if (!nw->isConnected || nw->clientSocket == INVALID_SOCKET) {
        return 0;
    }

    if (from < 0 || from > 99 || to < 0 || to > 99) {
        printf("Invalid move coordinates\n");
        return 0;
    }

    char moveMsg[32];
    int moveData = from * 100 + to;
    snprintf(moveMsg, sizeof(moveMsg), "\x01m %d", moveData);

    if (send(nw->clientSocket, moveMsg, strlen(moveMsg), 0) == SOCKET_ERROR) {
        printf("Move send failed: %d\n", WSAGetLastError());
        closeOnline(nw);
        return 0;
    }

    printf("Sent move: from %d to %d\n", from, to);
    return 1;
}


int onlineCamp(Network* nw, int pawn) {
    if (!nw->isConnected || nw->clientSocket == INVALID_SOCKET) {
        return 0;
    }

    if (pawn < 0 || pawn > 99) {
        printf("Invalid pawn number\n");
        return 0;
    }

    char campMsg[32];
    snprintf(campMsg, sizeof(campMsg), "\x02c %02d", pawn);

    if (send(nw->clientSocket, campMsg, strlen(campMsg), 0) == SOCKET_ERROR) {
        printf("Camp send failed: %d\n", WSAGetLastError());
        closeOnline(nw);
        return 0;
    }

    printf("Sent camp command for pawn %d\n", pawn);
    return 1;
}

#endif
