#include <stdio.h>
#include <stdlib.h>

void initPawns(short** pawns) {
    for (short i = 0; i < 8; i++) {
        pawns[0][i] = 2;
        pawns[7][i] = 1;
    }
}

short selectPawn(short** pawns, short turn, short x, short y, short printError) {
    if (pawns[x][y] == turn + 1) return 1;
    if (printError) showError("This is not your pawn");
    return 0;
}

short canPlace(short** pawns, short turn, short x, short y) {
    return pawns[x][y] != turn + 1;
}

void setMovable(short** pawns, short x, short y) {
    pawns[x][y] = pawns[x][y] + 3;
}

short canMove(short** board, short** pawns, short turn, short cell, short x, short y, short directReturn) {
    short i, j, cox, coy;

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
    } else if (cell == 3) {
        short moves[8][8];
        moves[0][0] = -1; moves[0][1] = -2;
        moves[1][0] = -2; moves[1][1] = -1;
        moves[2][0] = -2; moves[2][1] = 1;
        moves[3][0] = -1; moves[3][1] = 2;
        moves[4][0] = 1; moves[4][1] = -2;
        moves[5][0] = 2; moves[5][1] = -1;
        moves[6][0] = 2; moves[6][1] = 1;
        moves[7][0] = 1; moves[7][1] = 2;

        for (i = 0; i < 8; i++) {
            cox = x + moves[i][0];
            coy = y + moves[i][1];
            if (0 <= cox && cox < 8 && 0 <= coy && coy < 8 && canPlace(pawns, turn, cox, coy)) {
                if (directReturn) return 1;
                setMovable(pawns, cox, coy);
                cox += moves[i][0];
                coy += moves[i][1];
            }
        }

        return 1;
    } else {
        short moves[4][4];
        if (cell == 0) { // red
            moves[0][0] = 0; moves[0][1] = -1;
            moves[1][0] = -1; moves[1][1] = 0;
            moves[2][0] = 0; moves[2][1] = 1;
            moves[3][0] = 1; moves[3][1] = 0;

        } else { // yellwo
            moves[0][0] = -1; moves[0][1] = -1;
            moves[1][0] = -1; moves[1][1] = 1;
            moves[2][0] = 1; moves[2][1] = -1;
            moves[3][0] = 1; moves[3][1] = 1;
        }

        for (i = 0; i < 4; i++) {
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
    showError("This pawn cannot move.");
    return 0;
}

short moveToCamp(short** pawns, short turn, struct Player* players, short x, short y) {
    pawns[x][y] = 0;
    // Ici, pawns est affichable
}

short checkCamp(short** pawns, short turn, struct Player* players, short x, short y) {

    if ((turn == 0 && x == 0) || (turn == 1 && x == 7)) { // white on x0 || black on x7
        char buffer[1];
        do {
            inputStr("Move to camp? [Y/n]", buffer, 1, 1);
        } while (buffer[0] != 'Y' && buffer[0] != 'N');

        if (buffer[0] == 'Y') {
            moveToCamp(pawns, turn, players, x, y);
            return 1;
        }
    }
    return 0;
}

short selectMove(short** pawns, short x, short y) {
    if (pawns[x][y] >= 3) {
        return 1;
    }
    showError("You cannot move this pawn here.");
    return 0;
}

short switchTurn(short turn) {
    return 1 - turn;
}

void resetCells(short** pawns) {
    printf("im in: ");
    short i, j;
    for (i = 0; i < 8; i++) {
        printf("i");
        for (j = 0; j < 8; j++) {
            printf("j");
            if (pawns[i][j] > 2) {
                printf("IF");
                pawns[i][j] = pawns[i][j] - 3;
            }
        }
        printf("\n");
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
            if (selectPawn(pawns, oppTurn, i, j, 0)) nbOppPawn++;
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
        } while (!selectPawn(pawns, turn, x, y, 1) || !canMove(board, pawns, turn, board[x][y], x, y, 0));

        if (checkCamp(pawns, turn, players, x, y)) {
            if (players[turn].value == 2) break;
            // Ici, pawns n'est plus affichable
        } else {
            printf("\n");
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

    printf("Winner is player %d!", switchTurn(turn));
};

/* Cancel selection
Ira tjrs diag si rencontre pion
Roi mange pas*/
