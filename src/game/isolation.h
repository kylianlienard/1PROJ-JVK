// Game functions to follow Isolation game's logic

#ifndef fileISOLATION
#define fileISOLATION

#ifndef fileBOARD
#include "board.c"
#endif

#include <stdio.h>

void isolation_debug() {
    printf("Game Katarenga\n");
}

void isolation_use() {
    printf("Katarenga used\n");
}

#endif
// Vérifie si la case (y, x) est menacée par un pion déjà posé (toutes couleurs confondues)
int caseMenacee(int** board, int** pawns, int y, int x) {
    for (int py = 0; py < 8; py++) {
        for (int px = 0; px < 8; px++) {
            if (pawns[py][px] != 0) {
                int color = board[py][px];
                int original = pawns[y][x];
                pawns[y][x] = 0;
                setMoves(board, pawns, 0, color, px, py, 1, 0);
                int menaced = (pawns[y][x] > 2);
                clearMovable(pawns);
                pawns[y][x] = original;
                if (menaced)
                    return 1;
            }
        }
    }
    return 0;
}

// Renvoie 1 si au moins un coup est possible pour ce joueur
int coupsPossibles(int** board, int** pawns) {
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
            if (pawns[y][x] == 0 && !caseMenacee(board, pawns, y, x))
                return 1;
    return 0;
}

void isolationGame(SDL_Window* window, SDL_Renderer* renderer) {
    int** board = init8by8board();
    int** pawns = init8by8board();
    generateBoard(board);

    int turn = 0; // 0 ou 1
    int playing = 1;


    struct Player players[2];
    players[0].value = 0;
    players[1].value = 0;



    while (playing) {
        if (!coupsPossibles(board, pawns)) {
            printf("Le joueur %d ne peut plus jouer, le joueur %d gagne !\n", turn + 1, (1 - turn) + 1);
            break;
        }

        int action = 0;
        while (!action) {
            printf("Joueur %d, entrez les coordonnées x y (0-7 0-7) : ", turn + 1);
            int inputx, inputy;
            scanf("%d %d", &inputx, &inputy);

            if (inputx < 0 || inputx > 7 || inputy < 0 || inputy > 7)
                { printf("Coordonnées hors plateau.\n"); continue; }
            if (pawns[inputy][inputx] != 0)
                { printf("Case occupée.\n"); continue; }
            if (caseMenacee(board, pawns, inputy, inputx))
                { printf("Case menacée !\n"); continue; }

            pawns[inputy][inputx] = turn + 1;
            action = 1;
            if (renderer) {
                drawBoard(renderer, board, pawns, getShortestWindowEdge(window));
                SDL_RenderPresent(renderer);
            }
        }
        turn = 1 - turn;
    }

    for (int i = 0; i < 8; i++) { free(board[i]); free(pawns[i]); }
    free(board); free(pawns);
}
