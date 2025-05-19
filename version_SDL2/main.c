#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PORT 12345

/*---------------- Variables globales ----------------*/

// Mode Solo
char player1Text[100] = "";
char player2Text[100] = "";
int againstBot = 0;
int player1Active = 0;
int player2Active = 0;
int gameType = 0; // 0: Katarenga, 1: Congress, 2: Isolation

// Mode Online
char onlinePlayerName[100] = "";
int onlinePlayerActive = 0;   // Pour saisie du nom dans le menu Online
char onlineJoinIP[INET_ADDRSTRLEN] = "";
int onlineJoinActive = 0;     // Pour saisie de l'IP dans le mode Rejoindre
char opponentName[100] = "";  // Nom de l’adversaire reçu par le serveur
char serverIP[INET_ADDRSTRLEN] = ""; // Adresse IP du serveur
int onlineConnected = 0;      // Passe à 1 quand une connexion est établie

/*
  Valeurs de menuState :
    0 = Menu Principal,
    1 = Menu Mode de Jeu (Solo, Online, Load, QUIT),
    2 = Sélection des joueurs en mode Solo,
    3 = Menu Online (saisie du nom + boutons "Creer" et "Rejoindre"),
    4 = Online Serveur (page "Création" et attente d’un adversaire),
    5 = Online Client (page "Rejoindre" et saisie de l’IP)
*/

/*---------------- Prototypes ----------------*/
void showMainMenu(SDL_Renderer* renderer, int width, int height, TTF_Font* font);
void showGameModeMenu(SDL_Renderer* renderer, int width, int height, TTF_Font* font);
void showPlayerSelectionMenu(SDL_Renderer* renderer, int width, int height, TTF_Font* font);
void showOnlineMenu(SDL_Renderer* renderer, int width, int height, TTF_Font* font);
void showOnlineCreer(SDL_Renderer* renderer, int width, int height, TTF_Font* font);
void showOnlineJoin(SDL_Renderer* renderer, int width, int height, TTF_Font* font);

void* serverThreadFunc(void *arg);
void joinServer(const char* ip, const char* message);
int serverThreadFuncWrapper(void *arg);  // Pour SDL_CreateThread

// Fonction pour obtenir l'IP publique (la première adresse non-loopback)
char *getPublicIP() {
    static char publicIP[INET_ADDRSTRLEN];
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        strcpy(publicIP, "127.0.0.1");
        return publicIP;
    }
    struct hostent *hp = gethostbyname(hostname);
    if (hp == NULL) {
        strcpy(publicIP, "127.0.0.1");
        return publicIP;
    }
    int i = 0;
    // Parcourir les adresses et retourner la première qui ne commence pas par "127."
    for (; hp->h_addr_list[i] != NULL; i++) {
        struct in_addr addr;
        memcpy(&addr, hp->h_addr_list[i], sizeof(addr));
        const char *ipStr = inet_ntoa(addr);
        printf("on a %s\n", ipStr);
        if (strncmp(ipStr, "127.", 4) != 0 && strncmp(ipStr, "10.", 4) != 0 && strncmp(ipStr, "192.", 4) != 0) {
            strcpy(publicIP, ipStr);
            return publicIP;
        }
    }
    // Sinon, retourner la première adresse
    struct in_addr addr;
    memcpy(&addr, hp->h_addr_list[0], sizeof(addr));
    strcpy(publicIP, inet_ntoa(addr));
    return publicIP;
}

/*---------------- Fonctions d'affichage ----------------*/

void showMainMenu(SDL_Renderer* renderer, int width, int height, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 50, 150, 255, 255);
    SDL_Rect button = { width / 4, height / 3, width / 2, height / 5 };
    SDL_RenderFillRect(renderer, &button);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &button);

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Jouer", textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = { button.x + button.w / 4, button.y + button.h / 4,
                          textSurface->w, textSurface->h };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    SDL_RenderPresent(renderer);
}

void showGameModeMenu(SDL_Renderer* renderer, int width, int height, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
    SDL_RenderClear(renderer);

    char* options[] = { "Solo", "Online", "Load", "QUIT" };
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Rect buttons[4];

    for (int i = 0; i < 4; i++) {
        buttons[i].x = width / 4;
        buttons[i].y = (height / 6) * (i + 2);
        buttons[i].w = width / 2;
        buttons[i].h = height / 8;

        SDL_SetRenderDrawColor(renderer, 50, 150, 255, 255);
        SDL_RenderFillRect(renderer, &buttons[i]);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &buttons[i]);

        SDL_Surface* textSurface = TTF_RenderText_Solid(font, options[i], textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = { buttons[i].x + buttons[i].w / 4,
                              buttons[i].y + buttons[i].h / 4,
                              textSurface->w, textSurface->h };
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
    SDL_RenderPresent(renderer);
}

void showPlayerSelectionMenu(SDL_Renderer* renderer, int width, int height, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
    SDL_RenderClear(renderer);

    SDL_Color textColor = {0, 0, 0, 255};
    int fieldX = width / 4, fieldW = width / 2, fieldH = height / 10;
    int field1Y = height / 4, field2Y = field1Y + fieldH + 30;

    // Label Joueur 1
    SDL_Surface* labelSurface = TTF_RenderText_Solid(font, "Joueur 1:", textColor);
    SDL_Texture* labelTexture = SDL_CreateTextureFromSurface(renderer, labelSurface);
    SDL_Rect labelRect = { fieldX - 100, field1Y + 10, labelSurface->w, labelSurface->h };
    SDL_RenderCopy(renderer, labelTexture, NULL, &labelRect);
    SDL_FreeSurface(labelSurface);
    SDL_DestroyTexture(labelTexture);

    // Champ de saisie Joueur 1
    SDL_Rect field1 = { fieldX, field1Y, fieldW, fieldH };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &field1);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &field1);
    const char* displayText1 = (strlen(player1Text) == 0) ? " " : player1Text;
    labelSurface = TTF_RenderText_Solid(font, displayText1, textColor);
    labelTexture = SDL_CreateTextureFromSurface(renderer, labelSurface);
    SDL_Rect textRect = { field1.x + 5, field1.y + (field1.h - labelSurface->h) / 2,
                          labelSurface->w, labelSurface->h };
    SDL_RenderCopy(renderer, labelTexture, NULL, &textRect);
    SDL_FreeSurface(labelSurface);
    SDL_DestroyTexture(labelTexture);

    // Label Joueur 2
    labelSurface = TTF_RenderText_Solid(font, "Joueur 2:", textColor);
    labelTexture = SDL_CreateTextureFromSurface(renderer, labelSurface);
    labelRect.x = fieldX - 100;
    labelRect.y = field2Y + 10;
    labelRect.w = labelSurface->w;
    labelRect.h = labelSurface->h;
    SDL_RenderCopy(renderer, labelTexture, NULL, &labelRect);
    SDL_FreeSurface(labelSurface);
    SDL_DestroyTexture(labelTexture);

    // Champ de saisie Joueur 2
    SDL_Rect field2 = { fieldX, field2Y, fieldW, fieldH };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &field2);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &field2);
    const char* displayText2 = (strlen(player2Text) == 0) ? " " : player2Text;
    labelSurface = TTF_RenderText_Solid(font, displayText2, textColor);
    labelTexture = SDL_CreateTextureFromSurface(renderer, labelSurface);
    textRect.x = field2.x + 5;
    textRect.y = field2.y + (field2.h - labelSurface->h) / 2;
    textRect.w = labelSurface->w;
    textRect.h = labelSurface->h;
    SDL_RenderCopy(renderer, labelTexture, NULL, &textRect);
    SDL_FreeSurface(labelSurface);
    SDL_DestroyTexture(labelTexture);

    // Case à cocher "Jouer contre un bot"
    int botY = field2Y + fieldH + 20;
    SDL_Rect botCheckbox = { fieldX, botY, 20, 20 };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &botCheckbox);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &botCheckbox);
    SDL_Surface* botTextSurface = TTF_RenderText_Solid(font, "Jouer contre un bot", textColor);
    SDL_Texture* botTextTexture = SDL_CreateTextureFromSurface(renderer, botTextSurface);
    SDL_Rect botTextRect = { botCheckbox.x + botCheckbox.w + 10, botCheckbox.y,
                             botTextSurface->w, botTextSurface->h };
    SDL_RenderCopy(renderer, botTextTexture, NULL, &botTextRect);
    SDL_FreeSurface(botTextSurface);
    SDL_DestroyTexture(botTextTexture);

    if (againstBot) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawLine(renderer, botCheckbox.x, botCheckbox.y,
                           botCheckbox.x + botCheckbox.w, botCheckbox.y + botCheckbox.h);
        SDL_RenderDrawLine(renderer, botCheckbox.x + botCheckbox.w, botCheckbox.y,
                           botCheckbox.x, botCheckbox.y + botCheckbox.h);
    }

    // Sélection du type de jeu
    int gameTypeY = botY + 40;
    int margin = 10;
    int gameFieldX = fieldX - 50;
    int gameFieldWidth = fieldW + 100;
    int optionWidth = (gameFieldWidth - 2 * margin) / 3;
    int optionHeight = 40;
    const char* types[3] = {"Katarenga", "Congress", "Isolation"};

    for (int i = 0; i < 3; i++) {
        SDL_Rect optionRect = { gameFieldX + i * (optionWidth + margin), gameTypeY, optionWidth, optionHeight };
        if (i == gameType)
            SDL_SetRenderDrawColor(renderer, 150, 150, 255, 255);
        else
            SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
        SDL_RenderFillRect(renderer, &optionRect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &optionRect);

        SDL_Surface* gameTextSurface = TTF_RenderText_Solid(font, types[i], textColor);
        SDL_Texture* gameTextTexture = SDL_CreateTextureFromSurface(renderer, gameTextSurface);
        SDL_Rect gameTextRect;
        gameTextRect.x = optionRect.x + (optionRect.w - gameTextSurface->w) / 2;
        gameTextRect.y = optionRect.y + (optionRect.h - gameTextSurface->h) / 2;
        gameTextRect.w = gameTextSurface->w;
        gameTextRect.h = gameTextSurface->h;
        SDL_RenderCopy(renderer, gameTextTexture, NULL, &gameTextRect);

        SDL_FreeSurface(gameTextSurface);
        SDL_DestroyTexture(gameTextTexture);
    }

    // Bouton "Jouer"
    int playButtonY = gameTypeY + optionHeight + 20;
    SDL_Rect playButton = { fieldX, playButtonY, fieldW, height / 8 };
    SDL_SetRenderDrawColor(renderer, 50, 150, 255, 255);
    SDL_RenderFillRect(renderer, &playButton);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &playButton);
    SDL_Surface* playSurface = TTF_RenderText_Solid(font, "Jouer", textColor);
    SDL_Texture* playTexture = SDL_CreateTextureFromSurface(renderer, playSurface);
    SDL_Rect playRect = { playButton.x + (playButton.w - playSurface->w) / 2,
                          playButton.y + (playButton.h - playSurface->h) / 2,
                          playSurface->w, playSurface->h };
    SDL_RenderCopy(renderer, playTexture, NULL, &playRect);
    SDL_FreeSurface(playSurface);
    SDL_DestroyTexture(playTexture);

    SDL_RenderPresent(renderer);
}

void showOnlineMenu(SDL_Renderer* renderer, int width, int height, TTF_Font* font) {
    // Menu Online : saisie du nom et choix entre "Creer" et "Rejoindre"
    SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
    SDL_RenderClear(renderer);
    SDL_Color textColor = {0, 0, 0, 255};

    // Titre "Mode Online"
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "Mode Online", textColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect = { width / 2 - titleSurface->w / 2, height / 8 - titleSurface->h / 2,
                           titleSurface->w, titleSurface->h };
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    // Champ de saisie pour le nom du joueur
    int fieldX = width / 4, fieldW = width / 2, fieldH = height / 10;
    int fieldY = height / 4;
    SDL_Rect nameField = { fieldX, fieldY, fieldW, fieldH };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &nameField);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &nameField);
    const char* displayName = (strlen(onlinePlayerName) == 0) ? "Entrez votre nom" : onlinePlayerName;
    titleSurface = TTF_RenderText_Solid(font, displayName, textColor);
    titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect nameRect = { nameField.x + 5, nameField.y + (fieldH - titleSurface->h) / 2,
                          titleSurface->w, titleSurface->h };
    SDL_RenderCopy(renderer, titleTexture, NULL, &nameRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    // Bouton "Creer"
    int buttonW = fieldW / 2 - 15, buttonH = height / 10;
    SDL_Rect creerButton = { fieldX, fieldY + fieldH + 30, buttonW, buttonH };
    SDL_SetRenderDrawColor(renderer, 50, 150, 255, 255);
    SDL_RenderFillRect(renderer, &creerButton);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &creerButton);
    titleSurface = TTF_RenderText_Solid(font, "Creer", textColor);
    titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect creerRect = { creerButton.x + (creerButton.w - titleSurface->w) / 2,
                           creerButton.y + (creerButton.h - titleSurface->h) / 2,
                           titleSurface->w, titleSurface->h };
    SDL_RenderCopy(renderer, titleTexture, NULL, &creerRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    // Bouton "Rejoindre"
    SDL_Rect rejoindreButton = { fieldX + buttonW + 30, fieldY + fieldH + 30, buttonW, buttonH };
    SDL_SetRenderDrawColor(renderer, 50, 150, 255, 255);
    SDL_RenderFillRect(renderer, &rejoindreButton);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &rejoindreButton);
    titleSurface = TTF_RenderText_Solid(font, "Rejoindre", textColor);
    titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect rejoindreRect = { rejoindreButton.x + (rejoindreButton.w - titleSurface->w) / 2,
                               rejoindreButton.y + (rejoindreButton.h - titleSurface->h) / 2,
                               titleSurface->w, titleSurface->h };
    SDL_RenderCopy(renderer, titleTexture, NULL, &rejoindreRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    SDL_RenderPresent(renderer);
}

void showOnlineCreer(SDL_Renderer* renderer, int width, int height, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 200, 230, 200, 255);
    SDL_RenderClear(renderer);
    SDL_Color textColor = {0, 0, 0, 255};

    // Titre "Serveur - En attente..."
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "Serveur - En attente...", textColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect = { width / 2 - titleSurface->w / 2, height / 8,
                           titleSurface->w, titleSurface->h };
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    // Affichage du nom local
    char localMsg[150];
    snprintf(localMsg, sizeof(localMsg), "Votre nom : %s", onlinePlayerName);
    titleSurface = TTF_RenderText_Solid(font, localMsg, textColor);
    titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect localRect = { width / 2 - titleSurface->w / 2, height / 4,
                           titleSurface->w, titleSurface->h };
    SDL_RenderCopy(renderer, titleTexture, NULL, &localRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    // Affichage de l'IP du serveur (cette IP est désormais obtenue dynamiquement)
    char ipMsg[150];
    snprintf(ipMsg, sizeof(ipMsg), "Votre IP : %s", serverIP);
    titleSurface = TTF_RenderText_Solid(font, ipMsg, textColor);
    titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect ipRect = { width / 2 - titleSurface->w / 2, height / 4 + 50,
                        titleSurface->w, titleSurface->h };
    SDL_RenderCopy(renderer, titleTexture, NULL, &ipRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    // Affichage du nom de l'adversaire ou message d'attente
    if (onlineConnected) {
        char oppMsg[150];
        snprintf(oppMsg, sizeof(oppMsg), "Adversaire : %s", opponentName);
        titleSurface = TTF_RenderText_Solid(font, oppMsg, textColor);
    } else {
        titleSurface = TTF_RenderText_Solid(font, "En attente d'un adversaire...", textColor);
    }
    titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect oppRect = { width / 2 - titleSurface->w / 2, height / 2,
                         titleSurface->w, titleSurface->h };
    SDL_RenderCopy(renderer, titleTexture, NULL, &oppRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    SDL_RenderPresent(renderer);
}

void showOnlineJoin(SDL_Renderer* renderer, int width, int height, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 230, 230, 250, 255);
    SDL_RenderClear(renderer);
    SDL_Color textColor = {0, 0, 0, 255};

    // Titre "Rejoindre une partie"
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "Rejoindre une partie", textColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect = { width / 2 - titleSurface->w / 2, height / 8,
                           titleSurface->w, titleSurface->h };
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    // Champ de saisie pour l'IP du serveur
    int fieldX = width / 4, fieldW = width / 2, fieldH = height / 10;
    int fieldY = height / 3;
    SDL_Rect ipField = { fieldX, fieldY, fieldW, fieldH };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ipField);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &ipField);
    const char* displayIP = (strlen(onlineJoinIP) == 0) ? "Entrez l'IP du serveur" : onlineJoinIP;
    titleSurface = TTF_RenderText_Solid(font, displayIP, textColor);
    titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect ipRect = { ipField.x + 5, ipField.y + (fieldH - titleSurface->h) / 2,
                        titleSurface->w, titleSurface->h };
    SDL_RenderCopy(renderer, titleTexture, NULL, &ipRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    // Bouton "Rejoindre"
    SDL_Rect joinButton = { fieldX, fieldY + fieldH + 30, fieldW, fieldH };
    SDL_SetRenderDrawColor(renderer, 50, 150, 255, 255);
    SDL_RenderFillRect(renderer, &joinButton);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &joinButton);
    titleSurface = TTF_RenderText_Solid(font, "Rejoindre", textColor);
    titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect joinRect = { joinButton.x + (joinButton.w - titleSurface->w) / 2,
                          joinButton.y + (joinButton.h - titleSurface->h) / 2,
                          titleSurface->w, titleSurface->h };
    SDL_RenderCopy(renderer, titleTexture, NULL, &joinRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    SDL_RenderPresent(renderer);
}

/*---------------- Fonctions de réseau ----------------*/

// Fonction serveur : attend une connexion et reçoit le nom du client.
void* serverThreadFunc(void *arg) {
    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    int clilen;
    char buffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == INVALID_SOCKET) {
        fprintf(stderr, "Erreur lors de l'ouverture de la socket: %ld\n", WSAGetLastError());
        return NULL;
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        fprintf(stderr, "Erreur lors du bind: %ld\n", WSAGetLastError());
        closesocket(sockfd);
        return NULL;
    }

    if (listen(sockfd, 5) == SOCKET_ERROR) {
        fprintf(stderr, "Erreur lors de l'écoute: %ld\n", WSAGetLastError());
        closesocket(sockfd);
        return NULL;
    }

    // Utilisez la fonction getPublicIP pour obtenir l'adresse IP publique
    strncpy(serverIP, getPublicIP(), INET_ADDRSTRLEN);
    printf("Serveur : en attente de connexion sur le port %d...\n", PORT);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd == INVALID_SOCKET) {
        fprintf(stderr, "Erreur lors de l'acceptation: %ld\n", WSAGetLastError());
        closesocket(sockfd);
        return NULL;
    }
    memset(buffer, 0, sizeof(buffer));
    if (recv(newsockfd, buffer, sizeof(buffer) - 1, 0) == SOCKET_ERROR) {
        fprintf(stderr, "Erreur lors de la réception: %ld\n", WSAGetLastError());
        closesocket(newsockfd);
        closesocket(sockfd);
        return NULL;
    }
    printf("Message reçu : %s\n", buffer);
    strcpy(opponentName, buffer);
    onlineConnected = 1;
    closesocket(newsockfd);
    closesocket(sockfd);
    return NULL;
}

// Fonction client : se connecte au serveur et envoie le nom du joueur.
void joinServer(const char* ip, const char* message) {
    int sockfd;
    struct sockaddr_in serv_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == INVALID_SOCKET) {
        fprintf(stderr, "Erreur lors de l'ouverture de la socket: %ld\n", WSAGetLastError());
        return;
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        fprintf(stderr, "Adresse IP invalide\n");
        closesocket(sockfd);
        return;
    }
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        fprintf(stderr, "Erreur lors de la connexion: %ld\n", WSAGetLastError());
        closesocket(sockfd);
        return;
    }
    if (send(sockfd, message, (int)strlen(message), 0) == SOCKET_ERROR) {
        fprintf(stderr, "Erreur lors de l'envoi du message: %ld\n", WSAGetLastError());
    }
    printf("Message envoyé : %s\n", message);
    closesocket(sockfd);
}

// Wrapper pour SDL_CreateThread.
int serverThreadFuncWrapper(void *arg) {
    serverThreadFunc(arg);
    return 0;
}

/*---------------- Main ----------------*/
int main(int argc, char* argv[]) {
    // Initialisation de Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return -1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) {
        printf("Erreur d'initialisation SDL ou SDL_ttf: %s\n", SDL_GetError());
        WSACleanup();
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Page d'Accueil",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          600, 500, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Erreur de création de fenêtre: %s\n", SDL_GetError());
        SDL_Quit();
        WSACleanup();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erreur de création de renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        WSACleanup();
        return -1;
    }

    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        printf("Erreur lors du chargement de la police: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        WSACleanup();
        return -1;
    }

    int running = 1;
    SDL_Event event;
    // 0 = Main Menu, 1 = Mode de Jeu, 2 = Sélection Solo, 3 = Menu Online,
    // 4 = Online Serveur, 5 = Online Client
    int menuState = 0;
    SDL_Thread* serverThread = NULL;

    while (running) {
        int width, height;
        SDL_GetWindowSize(window, &width, &height);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = 0;

            // Menu Principal
            if (menuState == 0) {
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    int mouseX = event.button.x;
                    int mouseY = event.button.y;
                    SDL_Rect button = { width / 4, height / 3, width / 2, height / 5 };
                    if (mouseX >= button.x && mouseX <= button.x + button.w &&
                        mouseY >= button.y && mouseY <= button.y + button.h)
                    {
                        menuState = 1;
                    }
                }
            }
            // Mode de Jeu
            else if (menuState == 1) {
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    int mouseX = event.button.x;
                    int mouseY = event.button.y;
                    SDL_Rect soloButton   = { width / 4, (height / 6) * 2, width / 2, height / 8 };
                    SDL_Rect onlineButton = { width / 4, (height / 6) * 3, width / 2, height / 8 };
                    SDL_Rect loadButton   = { width / 4, (height / 6) * 4, width / 2, height / 8 };
                    SDL_Rect quitButton   = { width / 4, (height / 6) * 5, width / 2, height / 8 };

                    if (mouseX >= soloButton.x && mouseX <= soloButton.x + soloButton.w &&
                        mouseY >= soloButton.y && mouseY <= soloButton.y + soloButton.h) {
                        menuState = 2;  // Mode Solo
                    }
                    else if (mouseX >= onlineButton.x && mouseX <= onlineButton.x + onlineButton.w &&
                             mouseY >= onlineButton.y && mouseY <= onlineButton.y + onlineButton.h) {
                        menuState = 3;  // Menu Online
                    }
                    else if (mouseX >= loadButton.x && mouseX <= loadButton.x + loadButton.w &&
                             mouseY >= loadButton.y && mouseY <= loadButton.y + loadButton.h) {
                        printf("Bouton Load cliqué.\n");
                    }
                    else if (mouseX >= quitButton.x && mouseX <= quitButton.x + quitButton.w &&
                             mouseY >= quitButton.y && mouseY <= quitButton.y + quitButton.h) {
                        running = 0;
                    }
                }
            }
            // Sélection des joueurs en mode Solo
            else if (menuState == 2) {
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    int mouseX = event.button.x, mouseY = event.button.y;
                    int fieldX = width / 4, fieldW = width / 2, fieldH = height / 10;
                    int field1Y = height / 4, field2Y = field1Y + fieldH + 30;

                    if (mouseX >= fieldX && mouseX <= fieldX + fieldW &&
                        mouseY >= field1Y && mouseY <= field1Y + fieldH) {
                        player1Active = 1;
                        player2Active = 0;
                        SDL_StartTextInput();
                    }
                    else if (mouseX >= fieldX && mouseX <= fieldX + fieldW &&
                             mouseY >= field2Y && mouseY <= field2Y + fieldH) {
                        player1Active = 0;
                        player2Active = 1;
                        SDL_StartTextInput();
                    }
                    else {
                        player1Active = 0;
                        player2Active = 0;
                        SDL_StopTextInput();
                    }

                    // Case "Jouer contre un bot"
                    int botY = field2Y + fieldH + 20;
                    SDL_Rect botCheckbox = { fieldX, botY, 20, 20 };
                    if (mouseX >= botCheckbox.x && mouseX <= botCheckbox.x + botCheckbox.w &&
                        mouseY >= botCheckbox.y && mouseY <= botCheckbox.y + botCheckbox.h) {
                        againstBot = !againstBot;
                    }

                    // Sélection du type de jeu
                    int margin = 10;
                    int gameFieldX = fieldX - 50;
                    int gameFieldWidth = fieldW + 100;
                    int optionWidth = (gameFieldWidth - 2 * margin) / 3;
                    int optionHeight = 40;
                    int gameTypeY = botY + 40;
                    for (int i = 0; i < 3; i++) {
                        SDL_Rect optionRect = { gameFieldX + i * (optionWidth + margin), gameTypeY, optionWidth, optionHeight };
                        if (mouseX >= optionRect.x && mouseX <= optionRect.x + optionRect.w &&
                            mouseY >= optionRect.y && mouseY <= optionRect.y + optionRect.h)
                        {
                            gameType = i;
                        }
                    }

                    // Bouton "Jouer" pour lancer la partie Solo
                    int playButtonY = gameTypeY + optionHeight + 20;
                    SDL_Rect playButton = { fieldX, playButtonY, fieldW, height / 8 };
                    if (mouseX >= playButton.x && mouseX <= playButton.x + playButton.w &&
                        mouseY >= playButton.y && mouseY <= playButton.y + playButton.h)
                    {
                        printf("Lancement de la partie Solo...\n");
                        printf("Joueur 1 : %s\n", player1Text);
                        if (againstBot)
                            printf("Mode Bot activé pour Joueur 2.\n");
                        else
                            printf("Joueur 2 : %s\n", player2Text);
                        const char* types[3] = {"Katarenga", "Congress", "Isolation"};
                        printf("Type de jeu sélectionné : %s\n", types[gameType]);
                    }
                }
                if (event.type == SDL_TEXTINPUT) {
                    if (player1Active) {
                        if (strlen(player1Text) + strlen(event.text.text) < sizeof(player1Text) - 1)
                            strcat(player1Text, event.text.text);
                    } else if (player2Active) {
                        if (strlen(player2Text) + strlen(event.text.text) < sizeof(player2Text) - 1)
                            strcat(player2Text, event.text.text);
                    }
                }
                else if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_BACKSPACE) {
                        if (player1Active && strlen(player1Text) > 0)
                            player1Text[strlen(player1Text) - 1] = '\0';
                        else if (player2Active && strlen(player2Text) > 0)
                            player2Text[strlen(player2Text) - 1] = '\0';
                    }
                }
            }
            // Menu Online
            else if (menuState == 3) {
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    int mouseX = event.button.x, mouseY = event.button.y;
                    int fieldX = width / 4, fieldW = width / 2, fieldH = height / 10;
                    int fieldY = height / 4;
                    SDL_Rect nameField = { fieldX, fieldY, fieldW, fieldH };
                    if (mouseX >= nameField.x && mouseX <= nameField.x + nameField.w &&
                        mouseY >= nameField.y && mouseY <= nameField.y + nameField.h)
                    {
                        onlinePlayerActive = 1;
                        SDL_StartTextInput();
                    }
                    else {
                        onlinePlayerActive = 0;
                        SDL_StopTextInput();
                    }

                    // Bouton "Creer"
                    int buttonW = fieldW / 2 - 15, buttonH = height / 10;
                    SDL_Rect creerButton = { fieldX, fieldY + fieldH + 30, buttonW, buttonH };
                    if (mouseX >= creerButton.x && mouseX <= creerButton.x + creerButton.w &&
                        mouseY >= creerButton.y && mouseY <= creerButton.y + creerButton.h)
                    {
                        if (strlen(onlinePlayerName) > 0) {
                            serverThread = SDL_CreateThread(serverThreadFuncWrapper, "ServerThread", NULL);
                            if (serverThread == NULL) {
                                printf("Erreur de création du thread serveur: %s\n", SDL_GetError());
                            }
                            menuState = 4;
                        }
                    }

                    // Bouton "Rejoindre"
                    SDL_Rect rejoindreButton = { fieldX + buttonW + 30, fieldY + fieldH + 30, buttonW, buttonH };
                    if (mouseX >= rejoindreButton.x && mouseX <= rejoindreButton.x + rejoindreButton.w &&
                        mouseY >= rejoindreButton.y && mouseY <= rejoindreButton.y + rejoindreButton.h)
                    {
                        if (strlen(onlinePlayerName) > 0) {
                            menuState = 5;
                            onlineJoinActive = 1;
                            SDL_StartTextInput();
                        }
                    }
                }
                if (event.type == SDL_TEXTINPUT && onlinePlayerActive) {
                    if (strlen(onlinePlayerName) + strlen(event.text.text) < sizeof(onlinePlayerName) - 1)
                        strcat(onlinePlayerName, event.text.text);
                }
                else if (event.type == SDL_KEYDOWN && onlinePlayerActive) {
                    if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(onlinePlayerName) > 0)
                        onlinePlayerName[strlen(onlinePlayerName) - 1] = '\0';
                }
            }
            // Menu Online Serveur
            else if (menuState == 4) {
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                    menuState = 3;
                }
            }
            // Menu Online Client
            else if (menuState == 5) {
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    int mouseX = event.button.x, mouseY = event.button.y;
                    int fieldX = width / 4, fieldW = width / 2, fieldH = height / 10;
                    int fieldY = height / 3;
                    SDL_Rect ipField = { fieldX, fieldY, fieldW, fieldH };
                    if (mouseX >= ipField.x && mouseX <= ipField.x + ipField.w &&
                        mouseY >= ipField.y && mouseY <= ipField.y + ipField.h)
                    {
                        onlineJoinActive = 1;
                        SDL_StartTextInput();
                    }
                    else {
                        onlineJoinActive = 0;
                        SDL_StopTextInput();
                    }
                    SDL_Rect joinButton = { fieldX, fieldY + fieldH + 30, fieldW, fieldH };
                    if (mouseX >= joinButton.x && mouseX <= joinButton.x + joinButton.w &&
                        mouseY >= joinButton.y && mouseY <= joinButton.y + joinButton.h)
                    {
                        if (strlen(onlineJoinIP) > 0) {
                            joinServer(onlineJoinIP, onlinePlayerName);
                            menuState = 3;
                        }
                    }
                }
                if (event.type == SDL_TEXTINPUT && onlineJoinActive) {
                    if (strlen(onlineJoinIP) + strlen(event.text.text) < sizeof(onlineJoinIP) - 1)
                        strcat(onlineJoinIP, event.text.text);
                }
                else if (event.type == SDL_KEYDOWN && onlineJoinActive) {
                    if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(onlineJoinIP) > 0)
                        onlineJoinIP[strlen(onlineJoinIP) - 1] = '\0';
                }
            }
        } // Fin boucle d'événements

        // Rendu selon l'état
        if (menuState == 0)
            showMainMenu(renderer, width, height, font);
        else if (menuState == 1)
            showGameModeMenu(renderer, width, height, font);
        else if (menuState == 2)
            showPlayerSelectionMenu(renderer, width, height, font);
        else if (menuState == 3)
            showOnlineMenu(renderer, width, height, font);
        else if (menuState == 4)
            showOnlineCreer(renderer, width, height, font);
        else if (menuState == 5)
            showOnlineJoin(renderer, width, height, font);

        SDL_Delay(16);  // ~60
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    WSACleanup();
    return 0;
}
