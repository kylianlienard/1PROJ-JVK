// Toutes les fonctions pour créer le board et le manipuler.

unsigned short randint(short maxi) { return rand() % maxi; }
unsigned short rand8() { return rand() % 8; }

const short RED_DIRS[4][2] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}};
const short BLUE_DIRS[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
const short GREEN_DIRS[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
const short YELLOW_DIRS[8][2] = {{-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}, {1, -2}, {2, -1}, {2, 1}, {1, 2}};

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

short** init8by8board() {
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
};
