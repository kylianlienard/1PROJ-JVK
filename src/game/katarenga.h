// Game functions to follow Katarenga game's logic

#ifndef fileKATARENGA
#define fileKATARENGA

#ifndef fileBOARD
#include "board.c"

struct Player { // temp
    char name[20];
    short value;
};
#endif

#include <stdio.h>

void setKatarengaPawns(short** pawns) { /// nom sera commun
    for (short i = 0; i < 8; i++) {
        pawns[0][i] = 2;
        pawns[7][i] = 1;
    }
}

short canCamp(short turn, short x, short y) {
    return ((turn == 0 && x == 0) || (turn == 1 && x == 7));
}

short katarengaEnd(short** board, short** pawns, short turn, struct Player* players) {

    /*if (players[turn].value >= 2) {
        return 1;
    }*/

    short i, j;
    short nbOppPiece = 0;
    short oppPiece = 1 - turn + 1;

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            if (pawns[i][j] == oppPiece && setMoves(pawns, turn, board[i][j], i, j, 1, 1)) { // 1 est par logique, on ne perd pas le premier tour, peut causer des problèmes
                if (++nbOppPiece > 1) return 0;
            }
        }
    }

    return 1;
}

short katarenga(SDL_Window* window, SDL_Renderer* renderer, short** board, short** pawns, struct Player* players, short gridXY, short gameValue, short prevGridXY, short turn, short round) {

     short x = getSingleCoord(gridXY, 0);
     short y = getSingleCoord(gridXY, 1);

    if (prevGridXY == -1) { // Demande de selection
        printf(">selecting: %d;%d\n", x, y);
        if (!selectPawn(pawns, turn, x, y)) { showError("This is not your pawn."); return -1; }
        if (!setMoves(pawns, turn, board[x][y], x, y, round > 0, 0)) { showError("This pawn cannot move."); return -1; }
        display2Boards(board, pawns);
        return gridXY;
    } else { //demande de mouvement, camp sera verif si clic

        short pawnX = getSingleCoord(prevGridXY, 0);
        short pawnY = getSingleCoord(prevGridXY, 1);
        if (!canMove(pawns, x, y)) { showError("Your selected pawn cannot move here."); return -1; };
        printf(">moving: %d;%d to %d;%d\n", pawnX, pawnY, x, y);
        clearMovable(pawns);
        pawns[pawnX][pawnY] = 0;
        pawns[x][y] = turn + 1;

    }

    if (katarengaEnd(board, pawns, turn, players)) { return -2; };
    display2Boards(board, pawns);

    return 0;
}

#endif
