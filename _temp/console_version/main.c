#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

struct Player {
    char name[20];
    short value;
};

#include "katarenga.h"

void showError(const char *text) {
    printf("%s\n", text);
}

unsigned short randint(short maxi) { return ((uint64_t)rand() * maxi) >> 32; }
unsigned short rand8() { return rand() % 8; }

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

int inputInt(const char *question, int maxInt) {
    int answer;
    while (1) {
        printf("%s: ", question);
        if (scanf("%d", &answer) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        } else if (answer >= 0 || answer <= maxInt) {
            break;
        }
    }
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

char symbl(short value, short boardTab) {
    if (boardTab) {
        switch (value) {
            case 0: return '+';
            case 1: return 'O';
            case 2: return '*';
            case 3: return 'L';
            default: return '?';
        }
    } else {
        if (value == 0) {
            return '.';
        } else {
            return value + '0';
        }
    }
}

void displayBoard(short** board, short boardTab) { // si boardTab == 1 cela affichera les symboles pour "board",  pour "pawns"
    printf(" y ");
    for (short i = 0; i < 8; i++) {
        printf("%d ", i);
    }
    printf("\nx+----------------\n");

    for (short i = 0; i < 8; i++) {
        printf("%d| ", i);
        for (short j = 0; j < 8; j++) {
            printf("%c ", symbl(board[i][j], boardTab));
        }
        printf("\n");
    }
}

void shuffle(short *arr, short length) {
    short i, j, tmp;
    for (i = length - 1; i > 0; i--) {
        j = randint(i);
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
    if (1) { // si saved games
        const char *options[] = {"Solo", "Online", "Load", "Quit"};
        return displayChoice("What would you like to do?", 4, options);
    } else {
        const char *options[] = {"Solo", "Online", "Quit"};
        return displayChoice("What would you like to do?", 3, options);
    }

}

short choosePlay() { // Do I need to free play[]?
    const char *play[] = {"1 player", "2 players", "Go back"};
    return displayChoice("How will you play?", 3, play);
}

short chooseGame() {
    const char *games[] = {"Katarenga", "Congress", "Isolation", "Go back"};
    return displayChoice("What game will you play?", 4, games);
}

void initBoard(short** board) {
    const short quadrants[4][4][4] = {
        {{2, 1, 3, 0}, {0, 3, 2, 2}, {3, 0, 1, 1}, {1, 2, 0, 3}},
        {{3, 1, 2, 0}, {0, 1, 2, 3}, {2, 0, 3, 1}, {1, 3, 0, 2}},
        {{3, 1, 0, 2}, {1, 3, 1, 0}, {2, 0, 2, 3}, {0, 3, 2, 1}},
        {{2, 1, 3, 0}, {0, 1, 2, 2}, {1, 3, 0, 3}, {3, 0, 2, 1}}
    };

    short** quad = (short**)malloc(4 * sizeof(short*));

    if (!quad) {
        printf("ALLOC ERROR: quad\n");
        exit(1);
    }

    for (int i = 0; i < 4; i++) {
        quad[i] = (short*)malloc(4 * sizeof(short));
        if (!quad[i]) {
            printf("ALLOC ERROR: quad[i]\n");

            for (short j = 0; j < i; j++) {
                free(quad[j]);
            }
            free(quad);
            exit(1);
        }
    }

    short right, bottom;
    short indexes[4] = {0, 1, 2, 3};
    shuffle(indexes, 4);

    for (short i = 0; i < 4; i++) {
        for (short x = 0; x < 4; x++) {
            for (short y = 0; y < 4; y++) {
                quad[x][y] = quadrants[indexes[i]][x][y];
            }
        }

        if (randint(1)) {
            flipArray(quad);
        }

        for (short r = 0; r < randint(3); r++) {
            rotateArray90(quad);
        }

        right = (i > 1) ? 4 : 0;
        bottom = (i % 2 == 1) ? 4 : 0;

        for (short x = 0; x < 4; x++) {
            for (short y = 0; y < 4; y++) {
                board[x + right][y + bottom] = quad[x][y];
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        free(quad[i]);
    }
    free(quad);
}

short endWindow(struct Player* players, short** pawns, short** board) {
    free(players);

    for (int i = 0; i < 8; i++) {
        free(pawns[i]);
        free(board[i]);
    }
    free(pawns);
    free(board);

    return 0;
}

short** init8by8pawns() {
    short** eight = (short**)malloc(8 * sizeof(short*));
    if (!eight) {
        printf("ALLOC ERROR: eight\n");
        exit(1);
    }

    for (int i = 0; i < 8; i++) {
        eight[i] = (short*)malloc(8 * sizeof(short));
        if (!eight[i]) {
            printf("ALLOC ERROR: eight[i]\n");

            for (short j = 0; j < i; j++) {
                free(eight[j]);
            }
            free(eight);
            exit(1);
        }

        for (short j = 0; j < 8; j++) {
            eight[i][j] = 0;
        }
    }

    return eight;
}

int main() {
    short c1, c2, c3;
    srand(time(NULL));

    struct Player* players = (struct Player*)malloc(2 * sizeof(struct Player));
    if (!players) {
        printf("ALLOC ERROR: players\n");
        exit(1);
    }

    for (short plna = 0; plna < 2; plna++) {
        /*printf("Player %d name: ", plna + 1);
        scanf("%19s", players[plna].name);*/
        players[plna].value = 0;
    }

    short** board = init8by8pawns();
    short** pawns = init8by8pawns();

    /*while (1) {
        c1 = chooseStart();

        if (c1 == 1) {
            c2 = choosePlay();
            if (c2 == 4) {
                continue;
            }

            while (1) {
                c3 = chooseGame();
                if (c3 == 4) {
                    break;
                }

                // initBoard(board);

                if (c3 == 1) { // Katarenga
                    printf("Katarenga\n");*/
                    initBoard(board);
                    katarenga(board, pawns, players);
                /*} else if (c3 == 2) { // Congress
                    printf("Congress\n");
                } else { // Isolation
                    printf("Isolation\n");
                }

                char buffer[2];
                do {
                    inputStr("Play again? [Y/n]", buffer, 1, 1);
                } while (buffer[0] != 'Y' && buffer[0] != 'N' && buffer[0] != 'y' && buffer[0] != 'n');

                if (buffer[0] == 'Y' || buffer[0] == 'y') {
                    return moveToCamp(pawns, turn, players, x, y);
                } else {
                    for (short i = 0; i < 8; i++) {
                        free(board[i]);
                        free(pawns[i]);
                    }
                    free(board);
                    free(pawns);
                }

            }
        } else if (c1 == 2) {
            printf("Onlien\n");
        } else if (c1 == 3) {
            printf("load\n");
        } else {
            printf("GET OUT!\n");
            return endWindow(players, pawns, board);
        }
    }*/

    return endWindow(players, pawns, board);
}
