// Game functions to follow Conrgess game's logic

#ifndef fileCONGRESS
#define fileCONGRESS

#ifndef fileBOARD
#include "board.c"
#endif

#include <stdio.h>

void congress_debug() {
    printf("Game Katarenga\n");
}

void congress_use() {
    printf("Katarenga used\n");
}

#define EMPTY 0
#define WHITE 1
#define BLACK 2

void setCongressPawns(int pawns[8][8]) {
    int pattern[8] = {EMPTY, BLACK, EMPTY, WHITE, EMPTY, BLACK, EMPTY, WHITE};


        // Clear
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
            pawns[y][x] = EMPTY;

    // (row 0)
    for (int x = 0; x < 8; x++)
        pawns[0][x] = pattern[x];

    // (row 7)
    for (int x = 0; x < 8; x++)
        pawns[7][x] = pattern[x];

    // (col 0)
    for (int y = 0; y < 8; y++)
        pawns[y][0] = pattern[y];

    // (col 7)
    for (int y = 0; y < 8; y++)
        pawns[y][7] = pattern[y];
}

int canCamp(int turn, int x) {
    return ((turn == 0 && x == 0) || (turn == 1 && x == 7));
}

int CongressEnd(int pawns[8][8], int color) {
    for (int i = 0; i < 8; i++) {
        int hori = 0, vert = 0;
        for (int j = 0; j < 8; j++) {
            hori = (pawns[i][j] == color) ? hori + 1 : 0;
            vert = (pawns[j][i] == color) ? vert + 1 : 0;
            if (hori == 8 || vert == 8) return 1;
        }
    }
    return 0;
}
int katarenga(SDL_Window* window, SDL_Renderer* renderer, int** board, int** pawns, struct Player* players, int gridXY, int gameValue, int prevGridXY, int turn, int round) {

     int x = getSingleCoord(gridXY, 0);
     int y = getSingleCoord(gridXY, 1);

    if (prevGridXY == -1) { // Demande de selection
        printf(">Selecting: %d;%d\n", x, y);
        if (!selectPawn(pawns, turn, x, y)) { showError("This is not your pawn."); return -1; }
        if (!setMoves(board, pawns, turn, board[x][y], x, y, round > 0, 0)) { showError("This pawn cannot move."); return -1; }
        display2Boards(board, pawns);
        return gridXY;
    } else { //demande de mouvement, camp sera verif si clic
        int pawnX = getSingleCoord(prevGridXY, 0);
        int pawnY = getSingleCoord(prevGridXY, 1);
        if (!canMove(pawns, x, y)) { showError("Your selected pawn cannot move here."); return -1; };
        printf(">Moving: %d;%d to %d;%d\n", pawnX, pawnY, x, y);
        clearMovable(pawns);
        pawns[pawnX][pawnY] = 0;
        pawns[x][y] = turn + 1;
    }

        int playerColor = (turn == 0) ? WHITE : BLACK;
    if (CongressEnd(pawns, playerColor)) {
        printf("Player %d wins!\n", turn + 1);
        return -2;

    return 0;
}

#endif
