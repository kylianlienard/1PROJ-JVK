#include <stdio.h>
#include <stdlib.h>

void initPawns(short** pawns) {
    for (short i = 0; i < 8; i++) {
        pawns[0][i] = 2;
        pawns[7][i] = 1;
    }
}


short switchTurn(short turn) {
    return 1 - turn;
}

short selectPawn(short** pawns, short turn, short x, short y, short printError) {
    if (pawns[x][y] == turn + 1) return 1;
    if (printError) showError("This is not your pawn");
    return 0;
}

short canPlace(short** pawns, short turn, short x, short y, short round) {
    if (round < 1) return pawns[x][y] == switchTurn(turn);
    return pawns[x][y] != turn + 1;
}

void setMovable(short** pawns, short x, short y) {
    pawns[x][y] = pawns[x][y] + 3;
}

short correctCoord(short x, short y) {
    return 0 <= x && x < 8 && 0 <= y && y < 8;
}

short canMove(short** board, short** pawns, short turn, short cell, short x, short y, short directReturn, short round) {
    short i, j, cox, coy;

    if (cell == 1) { // bleu
        for (i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {
                if (i == 0 && j == 0) continue;
                cox = x + i;
                coy = y + j;
                if (correctCoord(cox, coy) && canPlace(pawns, turn, cox, coy, round)) {
                    if (directReturn) return 1;
                    setMovable(pawns, cox, coy);
                }
            }
        }
        return 1;
    } else if (cell == 3) {
        short moves[8][2] = {
            {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2},
            {1, -2}, {2, -1}, {2, 1}, {1, 2}
        };

        for (i = 0; i < 8; i++) {
            cox = x + moves[i][0];
            coy = y + moves[i][1];
            if (correctCoord(cox, coy) && canPlace(pawns, turn, cox, coy, round)) {
                if (directReturn) return 1;
                setMovable(pawns, cox, coy);
            }
        }
        return 1;
    } else {
        short moves[4][2];
        if (cell == 0) { // red
            short redMoves[4][2] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}};
            for (i = 0; i < 4; i++) {
                moves[i][0] = redMoves[i][0];
                moves[i][1] = redMoves[i][1];
            }
        } else { // yellow
            short yellowMoves[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
            for (i = 0; i < 4; i++) {
                moves[i][0] = yellowMoves[i][0];
                moves[i][1] = yellowMoves[i][1];
            }
        }

        for (i = 0; i < 4; i++) {
            cox = x + moves[i][0];
            coy = y + moves[i][1];
            while (correctCoord(cox, coy) && pawns[cox][coy] == 0) {
                if (directReturn) return 1;
                setMovable(pawns, cox, coy);
                cox += moves[i][0];
                coy += moves[i][1];
            }

            if (correctCoord(cox, coy) && pawns[cox][coy] != turn + 1) {
                if (directReturn) return 1;
                setMovable(pawns, cox, coy);
            }
        }
        return 1;
    }
    showError("This pawn cannot move.");
    return 0;
}

short moveToCamp(short** pawns, short turn, struct Player* players, short x, short y) {
    printf("curyurent urn %d", turn);
    pawns[x][y] = 0;
    players[turn].value++;
    printf("afetr affdding %d", players[turn].value);
    return 1;
}

short checkCamp(short** pawns, short turn, struct Player* players, short x, short y) {
    if ((turn == 0 && x == 0) || (turn == 1 && x == 7)) { // white on x0 || black on x7
        char buffer[2];
        do {
            inputStr("Move to camp? [Y/n]", buffer, 1, 1);
        } while (buffer[0] != 'Y' && buffer[0] != 'N' && buffer[0] != 'y' && buffer[0] != 'n');

        if (buffer[0] == 'Y' || buffer[0] == 'y') {
            return moveToCamp(pawns, turn, players, x, y);
        }
    }
    return 0;
}

short selectMove(short** pawns, short x, short y) {
    if (pawns[x][y] >= 3) return 1;
    showError("You cannot move this pawn here.");
    return 0;
}

void resetCells(short** pawns) {
    short i, j;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            if (pawns[i][j] > 2) {
                pawns[i][j] = pawns[i][j] - 3;
            }
        }
    }
}

short showBoard(short** board, short** pawns) {
    printf("Board:\n");
    displayBoard(board, 1);
    printf("Pawns:\n");
    displayBoard(pawns, 0);
    return 1;
}

short endGame(short** pawns, short turn, struct Player* players) {
    printf("QEUE %d", players[turn].value);
    if (players[turn].value >= 2) {
        return 1;
    }

    short oppTurn = switchTurn(turn);
    short nbOppPawn = 0;

    for (short i = 0; i < 8; i++) {
        for (short j = 0; j < 8; j++) {
            if (pawns[i][j] == oppTurn + 1) {
                if (nbOppPawn++ > 1) {
                    return 0;
                }
            }
        }
    }
    printf("Il rste %d pions", nbOppPawn);

    return 0;
}

void katarenga(short** board, short** pawns, struct Player* players) {
    short x, y, mox, moy;
    short turn = 0;
    short round = 0;

    initPawns(pawns);

    do {
        showBoard(board, pawns);
        printf("> Player %d's turn%s:\n", turn + 1, players[turn].value == 1 ? " (1 in camp)" : "");

        do {
            x = inputInt("Enter x [0-7]", 7);
            y = inputInt("Enter y [0-7]", 7);
        } while (!selectPawn(pawns, turn, x, y, 1) || !canMove(board, pawns, turn, board[x][y], x, y, 0, round));

        if (!checkCamp(pawns, turn, players, x, y)) {
            printf("\n");
            showBoard(board, pawns);

            do {
                mox = inputInt("Move to x [0-7]", 7);
                moy = inputInt("Move to y [0-7]", 7);
            } while (!selectMove(pawns, mox, moy));

            pawns[x][y] = 0;
            pawns[mox][moy] = turn + 1;
        }

        resetCells(pawns);
        turn = switchTurn(turn);
        round++;

    } while (!endGame(pawns, turn, players));

    printf("Winner is player %d!", turn + 1);
}
