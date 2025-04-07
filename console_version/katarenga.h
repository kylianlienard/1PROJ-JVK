#include <stdio.h>
#include <stdlib.h>

void initPawns(short** pawns) {
    for (short i = 0; i < 8; i++) {
        pawns[0][i] = 2;
        pawns[7][i] = 1;
    }
}

short selectPawn(short** pawns, short turn, short x, short y) {
    if (pawns[x][y] == turn + 1) {
        return 1;
    } else {
        showError("This is not your pawn");
        return 0;
    }
}

short canPlace(short** pawns, short turn, short x, short y) {
    return pawns[x][y] != turn + 1;
}

void setMovable(short** pawns, short x, short y) {
    pawns[x][y] = 3;
}

short canMove(short** board, short** pawns, short turn, short cell, short x, short y, short directReturn) {
    short i, j, cox, coy;

    pawns[x][y] = 4;
    if (cell == 1) { // bleu

        for (i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {
                if (i == 0 && j == 0) continue;
                cox = x + i;
                coy = y + j;
                if (0 <= cox && cox < 8 && 0 <= coy && coy < 8 && pawns[cox][coy] == 0) {
                    setMovable(pawns, cox, coy);
                }
            }
            if (directReturn) return 1;
        }
        return 1;
    } else {
        short nbChecks = 4; short moves[8][8];
        if (cell == 0) { // red
            moves[0][0] = 0; moves[0][1] = -1;
            moves[1][0] = -1; moves[1][1] = 0;
            moves[2][0] = 0; moves[2][1] = 1;
            moves[3][0] = 1; moves[3][1] = 0;

        } else if (cell == 2) { // yellwo
            moves[0][0] = -1; moves[0][1] = -1;
            moves[1][0] = -1; moves[1][1] = 1;
            moves[2][0] = 1; moves[2][1] = -1;
            moves[3][0] = 1; moves[3][1] = 1;

        } else { // geen
            moves[0][0] = -1; moves[0][1] = -2;
            moves[1][0] = -2; moves[1][1] = -1;
            moves[2][0] = -2; moves[2][1] = 1;
            moves[3][0] = -1; moves[3][1] = 2;
            moves[4][0] = 1; moves[4][1] = -2;
            moves[5][0] = 2; moves[5][1] = -1;
            moves[6][0] = 2; moves[6][1] = 1;
            moves[7][0] = 1; moves[7][1] = 2;
            nbChecks = 8;
        }

        for (i = 0; i < nbChecks; i++) {
            cox = x + moves[i][0];
            coy = y + moves[i][1];
            while (0 <= cox && cox < 8 && 0 <= coy && coy < 8 && canPlace(pawns, turn, cox, coy)) {
                if (directReturn) return 1;
                setMovable(pawns, cox, coy);
                cox += moves[i][0];
                coy += moves[i][1];
            }
        }
        return 1;
    }
    pawns[i][j] = turn + 1;
    showError("This pawn cannot move.");
    return 0;
}

short moveToCamp(short** pawns, short turn, struct Player* players, short x, short y) {
    pawns[x][y] = 0;
    if (--players[turn].value == 0) return 2;
    return 1;
}

short checkCamp(short** pawns, short turn, struct Player* players, short x, short y) {
    char buffer[1];

    if ((turn == 0 && x == 0) || (turn == 1 && x == 7)) { // white on x0 || black on x7
        do {
            inputStr("Move to camp? [Y/n]", buffer, 1, 1);
        } while (buffer[0] != 'Y' && buffer[0] != 'N');

        if (buffer[0] == 'Y') {
            return moveToCamp(pawns, turn, players, x, y);
        }
    }
    return 0;
}

short selectMove(short** pawns, short x, short y) {
    if (pawns[x][y] == 3) {
        return 1;
    } else {
        showError("You cannot move this pawn here.");
        return 0;
    }
}

short switchTurn(short turn) {
    return 1 - turn;
}

void resetCells(short** pawns) {
    for (short i = 0; i < 8; i++) {
        for (short j = 0; j < 8; j++) {
            if (pawns[i][j] > 2) pawns[i][j] = 0;
        }
    }
}

short showBoard(short** board, short** pawns) {
    printf("Board:\n");
    displayBoard(board, 1);
    printf("Pawns:\n");
    displayBoard(pawns, 0);
}

short endGame(short** pawns, short turn, struct Player* players) {
    if (players[turn].value == 2) {
        return 1;
    }

    short oppTurn = switchTurn(turn);
    short nbOppPawn = 0;

    for (short i = 0; i < 8; i++) {
        for (short j = 0; j < 8; j++) {
            if (selectPawn(pawns, oppTurn, i, j)) nbOppPawn++;
        }
    }

    if (nbOppPawn <= 1) { return 1;};
    return 0;
}

void katarenga(short** board, short** pawns, struct Player* players) {
    short turn = 0; short inCamp = 0; short round = 1;
    short x, y, mox, moy;

    initPawns(pawns);

    do {
        showBoard(board, pawns);
        printf("Player %d's turn:\n", turn + 1);
        do {
            x = inputInt("Enter x [0-7]");
            y = inputInt("Enter y [0-7]");
        } while (!selectPawn(pawns, turn, x, y) || !canMove(board, pawns, turn, board[x][y], x, y, 0));

        inCamp = checkCamp(pawns, turn, players, x, y);
        printf("\n");
        if (inCamp == 2) break;
        if (inCamp == 0) {
            showBoard(board, pawns);
            do {
                mox = inputInt("Move to x [0-7]");
                moy = inputInt("Move to y [0-7]");
            } while (!selectMove(pawns, mox, moy));

            pawns[x][y] = 0;
            pawns[mox][moy] = turn + 1;
        }

        resetCells(pawns);
        turn = switchTurn(turn);
        round++;
    } while (!endGame(pawns, turn, players));

    printf("Winner is player %d!", switchTurn(turn) + 1);
};

// tab wrong, + * no expand
