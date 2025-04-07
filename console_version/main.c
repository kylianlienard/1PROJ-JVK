#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct Player {
    char name[20];
    short value;
};

void showError(const char *text) {
    printf("%s\n", text);
}

#include "katarenga.h"

short randint(short mini, short maxi) {
    return (rand() % (maxi - mini + 1)) + mini;
}

short displayChoice(const char *question, const short numChoices, const char *choices[]) {
    short answer = 0;
    printf("+-- %s ---+\n", question);
    for (short i = 0; i < numChoices; i++) {
        printf("[%d] %s\n", i + 1, choices[i]);
    }
    while (answer < 1 || answer > numChoices) {
        printf("Choose an action: ");
        if (scanf("%hd", &answer) != 1) {
            while (getchar() != '\n');
            answer = 0;
        }
    }
    return answer;
}

int inputInt(const char *question) {
    int answer;
    printf("%s: ", question);
    scanf("%d", &answer);
    return answer;
}

void inputStr(const char *question, char *inputAns, int maxChar, int uppercase) {
    printf("\n%s: ", question);
    scanf("%s", inputAns);
    inputAns[maxChar] = '\0';
    if (uppercase) {
        for (int i = 0; inputAns[i]; i++) {
            if (inputAns[i] >= 'a' && inputAns[i] <= 'z') {
                inputAns[i] -= 32;
            }
        }
    }
}

char symbl(short value, short withPawns) {
    if (withPawns) {
        if (value == 0) {
            return '+';
        } else if (value == 1) {
            return 'O';
        } else if  (value == 2) {
            return '*';
        } else {
            return 'L';
        }
    } else {
        if (value == 0) {
            return '.';
        } else {
            return value + 48;
        }
    }
}

void displayBoard(short** board, short boarded) {
    for (short i = 0; i < 8; i++) {
        for (short j = 0; j < 8; j++) {
            printf("%c ", symbl(board[i][j], boarded));
        }
        printf("\n");
    }
    printf("\n");
}

void shuffle(short *arr, short length) {
    short i, j, tmp;
    for (i = length - 1; i > 0; i--) {
        j = randint(0, i);
        tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

void rotateArray90(short** arr) {
    for (short i = 0; i < 2; i++) {
        for (short j = i; j < 3 - i; j++) {
            short tmp = arr[i][j];
            arr[i][j] = arr[3-j][i];
            arr[3-j][i] = arr[3-i][3-j];
            arr[3-i][3-j] = arr[j][3-i];
            arr[j][3-i] = tmp;
        }
    }
}

void flipArray(short** arr) {
    for (short i = 0; i < 4; i++) {
        for (short j = 0; j < 2; j++) {
            short tmp = arr[i][j];
            arr[i][j] = arr[i][3-j];
            arr[i][3-j] = tmp;
        }
    }
}

short chooseStart() {
    const char *options[] = {"Create New Game", "Load Game", "Customize", "Quit"};
    return displayChoice("What would you like to do?", 4, options);
}

short choosePlay() {
    const char *play[] = {"Solo", "Local", "Online", "Go back"};
    return displayChoice("How will you play?", 4, play);
}

short chooseGame() {
    const char *games[] = {"Katarenga", "Congress", "Isolation", "Go back"};
    return displayChoice("What game will you play?", 4, games);
}

void initTab(short** board) {
    const short quadrants[4][4][4] = {
        {{2, 1, 3, 0}, {0, 3, 2, 2}, {3, 0, 1, 1}, {1, 2, 0, 3}},
        {{3, 1, 2, 0}, {0, 1, 2, 3}, {2, 0, 3, 1}, {1, 3, 0, 2}},
        {{3, 1, 0, 2}, {1, 3, 1, 0}, {2, 0, 2, 3}, {0, 3, 2, 1}},
        {{2, 1, 3, 0}, {0, 1, 2, 2}, {1, 3, 0, 3}, {3, 0, 2, 1}}
    };

    short** quad = (short**)malloc(4 * sizeof(short*));
    for (int i = 0; i < 4; i++) {
        quad[i] = (short*)malloc(4 * sizeof(short));
    }

    short right, bottom;
    short indexes[4] = {0, 1, 2, 3};
    shuffle(indexes, 4);

    for (short i = 0; i < 4; i++) {
        short x, y;
        for (x = 0; x < 4; x++) {
            for (y = 0; y < 4; y++) {
                quad[x][y] = quadrants[indexes[i]][x][y];
            }
        }

        if (randint(0, 1)) {
            flipArray(quad);
        }

        for (short r = 0; r < randint(0, 3); r++) {
            rotateArray90(quad);
        }

        right = (i > 1) ? 4 : 0;
        bottom = (i % 2 == 1) ? 4 : 0;

        for (x = 0; x < 4; x++) {
            for (y = 0; y < 4; y++) {
                board[x + right][y + bottom] = quad[x][y];
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        free(quad[i]);
    }
    free(quad);
}

short endWindow(struct Player* players, short** board, short** pawns) {
    free(players);

    for (int i = 0; i < 8; i++) {
        free(board[i]);
        free(pawns[i]);
    }
    free(board);
    free(pawns);

    return 0;
}

short** init8by8board() {
    short** board = (short**)malloc(8 * sizeof(short*));
    if (!board) {
        printf("pas d'alloc1\n");
        exit(1);
    }

    for (int i = 0; i < 8; i++) {
        board[i] = (short*)malloc(8 * sizeof(short));
        if (!board[i]) {
            printf("pas d'alloc2\n");

            for (int j = 0; j < i; j++) {
                free(board[j]);
            }
            free(board);
            exit(1);
        }

        for (int j = 0; j < 8; j++) {
            board[i][j] = 0;
        }
    }

    return board;
}

int main() {
    short c1, c2, c3;
    srand(time(NULL));

    short** board = init8by8board();
    short** pawns = init8by8board();

    struct Player* players = (struct Player*)malloc(2 * sizeof(struct Player));
    if (!players) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    char plname[21];

    for (short plna = 0; plna < 2; plna++) {
        //\ inputStr("What is your name? ", plname, 21, 0);
        strcpy(players[plna].name, "NIG");
        players[plna].value = 0;
    }

    /*while (1) {
        c1 = chooseStart();

        if (c1 == 1) {
            c2 = choosePlay();
            if (c2 == 4) {
                continue;
            }

            while(1) {
                c3 = chooseGame();
                if (c3 == 4) {
                    break;
                }

                for (int i = 0; i < 8; i++) {
                    for (int j = 0; j < 8; j++) {
                        board[i][j] = 0;
                        pawns[i][j] = 0;
                    }
                }*/

                initTab(board);

                /*if (c3 == 1) {*/
                    printf("Entering Katarenga\n");
                    katarenga(board, pawns, players);
                /*} else if (c3 == 2) {
                    printf("Congress\n");
                    displayBoard(board, 0);
                } else {
                    printf("Isolation\n");
                    displayBoard(board, 0);
                }
            }

        } else if (c1 == 2) {
            printf("Loading game...\n");
        } else if (c1 == 3) {
            printf("Customization options\n");
        } else {
            printf("Get out!\n");
            return endWindow(players, board, pawns);
        }
    }*/

    free(players);

    //endWindow(players, board, pawns);
    return 0;
}
