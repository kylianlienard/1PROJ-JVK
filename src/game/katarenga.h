// Fonctions pour jeu de kataFre bfa

void initGame(short** pawns) { /// nom sera commun
    for (short i = 0; i < 8; i++) {
        pawns[0][i] = 2;
        pawns[7][i] = 1;
    }
}

short setMoves(short** board, short** pawns, short turn, short x, short y, short directReturn, short round) { /// sera commun SANS LE ROUND
    short cell = board[x][y];
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

            if (correctCoord(cox, coy) && pawns[cox][coy] == 1 - turn + 1) {
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
    pawns[x][y] = 0;
    players[turn].value++;
    return 1;
}

short checkCamp(short** pawns, short turn, struct Player* players, short x, short y) {
    if ((turn == 0 && x == 0) || (turn == 1 && x == 7)) {
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


short gameEnd(short** pawns, short turn, struct Player* players) { /// nom sera commun

    if (players[turn].value >= 2) {
        return 1;
    }

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

void katarenga(short** board, short** pawns, struct Player* players) {
    short turn = 0; // joueur actuel 0 - 1
    short round = 0; // tour, seulement utile pour katarenga
    short x, y, mox, moy;

    initGame(pawns); // place les pions de base

    do {
        display2Boards(board, pawns);
        // printf("> Player %d's turn%s:\n", turn + 1, players[turn].value == 1 ? " (1 in camp)" : "");
        printf("> Player %d's turn, %d PIONS RENTRES:\n", turn + 1, players[turn].value);

        do {
            x = inputInt("Enter x [0-7]", 7); // 7 est le max de la valeur
            y = inputInt("Enter y [0-7]", 7);
        } while (!selectPawn(pawns, turn, x, y, 1) || !setMoves(board, pawns, turn, x, y, 0, round));

        if (!checkCamp(pawns, turn, players, x, y)) {
            printf("\n");
            display2Boards(board, pawns);

            do {
                mox = inputInt("Move to x [0-7]", 7);
                moy = inputInt("Move to y [0-7]", 7);
            } while (!possibleMove(pawns, mox, moy));

            pawns[x][y] = 0;
            pawns[mox][moy] = turn + 1;
        }

        clearMovable(pawns);

        if (gameEnd(pawns, turn, players)) break;

        turn = switchTurn(turn);
        if (turn == 0) round++;

    } while (1);

    printf("Winner is player %d!\n", turn + 1);
}
