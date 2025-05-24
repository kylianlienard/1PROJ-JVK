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

short katarengaEnd(short** pawns, short turn, struct Player* players) {

    /*if (players[turn].value >= 2) {
        return 1;
    }*/

    short nbOppPiece = 0;
    short oppPiece = 1 - turn + 1;

    for (short i = 0; i < 8; i++) {
        for (short j = 0; j < 8; j++) {
            if (pawns[i][j] == oppPiece) {
                if (++nbOppPiece > 1) return 0;
            }
        }
    }

    return 1;
}

short katarenga(SDL_Window* window, SDL_Renderer* renderer, short** board, short** pawns, struct Player* players, short x, short y, short gameState, short turn, short round) {

    if (gameState == 0) { // Demande de selection
        printf("selecting %d %d;\n", x, y);
        if (!selectPawn(pawns, turn, x, y)) { showError("Ceci n'est pas votre pion."); return gameState; }
        // if (!setMoves(board, pawns, turn, x, y, 0, round)) { showError("Ce pion ne peux pas être bougé."); return gameState; }

        return x * 10 + y + 1;
    } else { //demande de mouvement, camp sera verif si clic

        if (!canMove(pawns, x, y)) showError("Your selected pawn cannot move here.");
        gameState--;
        short pawnX = gameState % 10;
        short pawnY = pawnX - gameState;
        pawns[pawnX][pawnY] = 0;
        pawns[x][y] = turn + 1;

        printf("demande de mv\n");
    }

    if (katarengaEnd(pawns, turn, players)) { return -1; };

    return 0;
}

#endif
