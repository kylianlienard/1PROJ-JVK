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

short switchTurn(short turn) { /// sera commun
    return 1 - turn;
}

short selectPawn(short** pawns, short turn, short x, short y, short printError) {
    if (pawns[x][y] == turn + 1) return 1;
    if (printError && pawns[x][y] == 1 - turn + 1) showError("This is not your pawn");
    else if (printError) showError("There is no pawn here");
    return 0;
}

short correctCoord(short x, short y) { /// sera commun
    return 0 <= x && x < 8 && 0 <= y && y < 8; // peut-être à remove plus tard si prend trop de place
}

short canPlace(short** pawns, short turn, short x, short y, short round) { /// sera commun SANS LE ROUND
    if (!correctCoord(x, y)) return 0;
    if (round < 1) return pawns[x][y] == 0;
    return (pawns[x][y] == 0) || (pawns[x][y] != turn + 1);
}

void setMovable(short** pawns, short x, short y) { /// sera commun
    pawns[x][y] += 3;
}

short possibleMove(short** pawns, short x, short y) { /// sera commun
    if (pawns[x][y] > 2) return 1;
    showError("Your pawn cannot move here");
    return 0;
}

void clearMovable(short** pawns) { /// sera commun
    for (short i = 0; i < 8; i++) {
        for (short j = 0; j < 8; j++) {
            if (pawns[i][j] > 2) {
                pawns[i][j] -= 3;
            }
        }
    }
}

void display2Boards(short** board, short** pawns) { /// sera commun
    printf("Board:\n");
    displayBoard(board, 1); // board sera tjrs accomp de 1 et pawns avec 0
    printf("\nPawns:\n");
    displayBoard(pawns, 0);
    printf("\n");
}
