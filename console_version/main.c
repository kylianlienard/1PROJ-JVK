#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "katarenga.h"

struct Player {
    char name[20];
    short remainPawns;
    short tmp;
}

short randint(short mini, short maxi) {
    return (rand() % (maxi - mini + 1)) + mini;
}

short displayChoice(const char *question, const short numChoices, const char *choices[]) {
    short answer = 0;
    printf("\n+-- %s ---+\n", question);
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
    printf("\n%s: ", question);
    scanf("%d", &answer);
    return answer;
}

void displayBoard(short (*board)[8]) {
    for (short i = 0; i < 8; i++) {
        for (short j = 0; j < 8; j++) {
            printf("%d ", board[i][j]);
        }
        printf("\n");
    }
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

void rotateArray90(short (*arr)[4]) {
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

void flipArray(short (*arr)[4]) {
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

void initTab(short *board) {
    const short quadrants[4][4][4] = {
        {{2, 1, 3, 0}, {0, 3, 2, 2}, {3, 0, 1, 1}, {1, 2, 0, 3}},
        {{3, 1, 2, 0}, {0, 1, 2, 3}, {2, 0, 3, 1}, {1, 3, 0, 2}},
        {{3, 1, 0, 2}, {1, 3, 1, 0}, {2, 0, 2, 3}, {0, 3, 2, 1}},
        {{2, 1, 3, 0}, {0, 1, 2, 2}, {1, 3, 0, 3}, {3, 0, 2, 1}}
    };
    short quad[4][4];
    short right, bottom;

    short indexes[4] = {0, 1, 2, 3};
    shuffle(indexes, 4);
    printf("indexes %d %d %d %d\n\n", indexes[0], indexes[1], indexes[2], indexes[3]);
    
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
}

int main() {
    srand(time(NULL));
    short c1, c2, c3;
    
    while (1) {
    c1 = chooseStart();
    
    if (c1 == 1) {
        c2 = choosePlay();
        if (c2 == 4) {
            break;
        }

        c3 = chooseGame();
        if (c3 == 4) {
            break;
        }

        short board[8][8];
        short pawns[8][8];
        initTab(board);
        
        if (c3 == 1) {
            printf("Katarenga\n");
            displayBoard(board);
            katarenga(board, pawns);
        } else if (c3 == 2) {
            printf("Congress\n");
            displayBoard(board);
        } else {
            printf("Isolation\n");
            displayBoard(board);
        }
    } else if (c1 == 2) {
        printf("Loading game...\n");
    } else if (c1 == 3) {
        printf("Customization options\n");
    } else {
        printf("Get out!\n");
        return 0;
    }
    return 0;
    }
}