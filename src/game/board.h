// Manage the game's board data + general game functions

#ifndef fileBOARD
#define fileBOARD

#ifndef fileBOARD_UI
#include "board_ui.h"
#endif

struct Player;
int selectPawn(int** pawns, int turn, int x, int y);
int canMove(int** pawns, int x, int y);
int gameLoop(SDL_Window* window, SDL_Renderer* renderer, int gameType, int playingGame, struct Player* players, int theme);
int switchTurn(int turn);
int getSingleCoord(int clickXY, int coord);
int setMoves(int** board, int** pawns, int turn, int cell, int x, int y, int canEat, int directReturn);
int canCamp(int turn, int x);
//int campClick(int x, int y, int sWE);

unsigned int randint(int maxi) { return rand() % maxi; }
unsigned int rand8() { return rand() % 8; }

#ifndef fileKATARENGA
#include "katarenga.h"
#endif

#ifndef fileCONGRESS
#include "congress.h"
#endif


//- Constants -//

/*const int RED_DIRS[4][2] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}};
const int BLUE_DIRS[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
const int GREEN_DIRS[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
const int YELLOW_DIRS[8][2] = {{-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}, {1, -2}, {2, -1}, {2, 1}, {1, 2}};*/

//- Board generation -//

int** init8by8board() {
    int** eight = (int**)malloc(8 * sizeof(int*));
    if (!eight) {
        d("ALLOC ERROR: eight\n");
        exit(1);
    }

    for (int i = 0; i < 8; i++) {
        eight[i] = (int*)malloc(8 * sizeof(int));
        if (!eight[i]) {
            d("ALLOC ERROR: eight[i]\n");

            for (int j = 0; j < i; j++) {
                free(eight[j]);
            }
            free(eight);
            exit(1);
        }

        for (int j = 0; j < 8; j++) {
            eight[i][j] = 0;
        }
    }

    return eight;
};

void shuffle(int *arr, int length) {
    int i, j, tmp;
    for (i = length - 1; i > 0; i--) {
        j = randint(i);
        tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

void flipArray(int** arr) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 2; j++) {
            int tmp = arr[i][j];
            arr[i][j] = arr[i][3-j];
            arr[i][3-j] = tmp;
        }
    }
}

void rotateArray90(int** arr) {
    for (int i = 0; i < 2; i++) {
        for (int j = i; j < 3 - i; j++) {
            int tmp = arr[i][j];
            arr[i][j] = arr[3-j][i];
            arr[3-j][i] = arr[3-i][3-j];
            arr[3-i][3-j] = arr[j][3-i];
            arr[j][3-i] = tmp;
        }
    }
}

void generateBoard(int** board) {
    const int quadrants[4][4][4] = {
        {{2, 1, 3, 0}, {0, 3, 2, 2}, {3, 0, 1, 1}, {1, 2, 0, 3}},
        {{3, 1, 2, 0}, {0, 1, 2, 3}, {2, 0, 3, 1}, {1, 3, 0, 2}},
        {{3, 1, 0, 2}, {1, 3, 1, 0}, {2, 0, 2, 3}, {0, 3, 2, 1}},
        {{2, 1, 3, 0}, {0, 1, 2, 2}, {1, 3, 0, 3}, {3, 0, 2, 1}}
    };

    int** quad = (int**)malloc(4 * sizeof(int*));

    if (!quad) {
        d("ALLOC ERROR: quad\n");
        exit(1);
    }

    for (int i = 0; i < 4; i++) {
        quad[i] = (int*)malloc(4 * sizeof(int));
        if (!quad[i]) {
            d("ALLOC ERROR: quad[i]\n");

            for (int j = 0; j < i; j++) {
                free(quad[j]);
            }
            free(quad);
            exit(1);
        }
    }

    int right, bottom;
    int indexes[4] = {0, 1, 2, 3};
    shuffle(indexes, 4);

    for (int i = 0; i < 4; i++) {
        for (int x = 0; x < 4; x++) {
            for (int y = 0; y < 4; y++) {
                quad[x][y] = quadrants[indexes[i]][x][y];
            }
        }

        if (randint(1)) {
            flipArray(quad);
        }

        for (int r = 0; r < randint(3); r++) {
            rotateArray90(quad);
        }

        right = (i > 1) ? 4 : 0;
        bottom = (i % 2 == 1) ? 4 : 0;

        for (int x = 0; x < 4; x++) {
            for (int y = 0; y < 4; y++) {
                board[x + right][y + bottom] = quad[x][y];
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        free(quad[i]);
    }
    free(quad);
}

//- Game functions -//

int getSingleCoord(int clickXY, int coord) {
    return coord ? (clickXY - (clickXY % 10)) / 10 : clickXY % 10;
}

int correctCoord(int x, int y) {
    return 0 <= x && x < 8 && 0 <= y && y < 8;
}

int selectPawn(int** pawns, int turn, int x, int y) { // Erreurs + specifiques
    return pawns[x][y] == turn + 1;
}

int canMove(int** pawns, int x, int y) {
    return pawns[x][y] > 2;
}

void setMovable(int** pawns, int x, int y) {
    pawns[x][y] += 3;
    // update GUI
}

int canPlace(int** pawns, int turn, int x, int y, int canEat) {
    return (pawns[x][y] == 0) || (canEat && pawns[x][y] == (1 - turn) + 1);
}

int switchTurn(int turn) {
    return 1 - turn;
}

int setMoves(int** board, int** pawns, int turn, int cell, int x, int y, int canEat, int directReturn) { /// sera commun SANS LE ROUND
    int i, j, cox, coy;
    printf(">>>>canEat: %d\n", canEat);

    if (cell == 1) { // bleu
        for (i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {
                if (i == 0 && j == 0) continue;
                cox = x + i;
                coy = y + j;
                if (correctCoord(cox, coy)) {
                    printf("this coorrect? %d", canPlace(pawns, turn, cox, coy, canEat));
                }
                if (correctCoord(cox, coy) && canPlace(pawns, turn, cox, coy, canEat)) {
                    if (directReturn) return 1;
                    setMovable(pawns, cox, coy);
                }
            }
        }
        return 1;
    } else if (cell == 3) {
        int moves[8][2] = {
            {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2},
            {1, -2}, {2, -1}, {2, 1}, {1, 2}
        };

        for (i = 0; i < 8; i++) {
            cox = x + moves[i][0];
            coy = y + moves[i][1];
            if (correctCoord(cox, coy)) {
                    printf("this coorrect pawn? %d", canPlace(pawns, turn, cox, coy, canEat));
                }
            if (correctCoord(cox, coy) && canPlace(pawns, turn, cox, coy, canEat)) {
                if (directReturn) return 1;
                setMovable(pawns, cox, coy);
            }
        }
        return 1;
    } else {
        int moves[4][2];
        if (cell == 0) { // red
            int redMoves[4][2] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}};
            for (i = 0; i < 4; i++) {
                moves[i][0] = redMoves[i][0];
                moves[i][1] = redMoves[i][1];
            }
        } else { // yellow
            int yellowMoves[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
            for (i = 0; i < 4; i++) {
                moves[i][0] = yellowMoves[i][0];
                moves[i][1] = yellowMoves[i][1];
            }
        }

        for (i = 0; i < 4; i++) {
            cox = x + moves[i][0];
            coy = y + moves[i][1];
            while (correctCoord(cox, coy) && !(board[cox][coy] == cell) && pawns[cox][coy] == 0) {
                if (directReturn) return 1;
                setMovable(pawns, cox, coy);
                cox += moves[i][0];
                coy += moves[i][1];
            }
            if (correctCoord(cox,coy)) {
                printf(">>>>>choosing to eat for %d;%d: %d || %d\n", cox, coy, board[cox][coy] == cell, canEat && pawns[cox][coy] == 1 - turn + 1);
            }

            if (correctCoord(cox,coy) && canPlace(pawns, turn, cox, coy, canEat)) {
                if (directReturn) return 1;
                setMovable(pawns, cox, coy);
            }
        }
        return 1;
    }
    showError("This pawn cannot move.");
    return 0;
}

void clearMovable(int** pawns) {
    int i, j;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            if (pawns[i][j] > 2) {
                pawns[i][j] -= 3;
            }
        }
    }
}

void redrawBoard(SDL_Renderer* renderer, int** board, int** pawns, int sWE, int theme) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    drawBoard(renderer, board, pawns, sWE, theme);
    //drawUI()

    SDL_RenderPresent(renderer);
    d("redrawn");
}

// Block quand c'est mon tour, gerer IA
int gameLoop(SDL_Window* window, SDL_Renderer* renderer, int gameType, int playingGame, struct Player* players, int theme) { //0 local, 1 IA, 2 network
    SDL_Event event;
    int gameValue = -1; // 0 rien, 1 un pion est select, -2 si fin, -1 si joeuru rejoue
    int prevGameValue = -1; // Si vide, donc coup d'avant pas enregistré
    int turn = 0; // Current game turn
    int yourTurn = 0; // The player's turn
    int round = 0;
    int sWE = getShortestWindowEdge(window);
    int gridXY = -1;

    /*struct Player* players = (struct Player*)malloc(2 * sizeof(struct Player));
    if (!players) {
        printf("ALLOC ERROR: players\n");
        exit(1);
    }

    for (int plna = 0; plna < 2; plna++) {
        printf("Player %d name: ", plna + 1);
        scanf("%32s", players[plna].name);
        players[plna].value = 0;
    }*/

    int** board = init8by8board();
    int** pawns = init8by8board();

    generateBoard(board);

    switch(playingGame) {
    case 0:
        setKatarengaPawns(pawns); break;
    case 1:
        setCongressPawns(pawns); break;
    }

    drawBoard(renderer, board, pawns, sWE, theme);
    SDL_RenderPresent(renderer);

    while (1) {
        while (SDL_PollEvent(&event)) {
            switch(event.type){
            case SDL_QUIT:
                //Disconnect si online
                return 0;
            case SDL_WINDOWEVENT:
                sWE = getShortestWindowEdge(window);
                redrawBoard(renderer, board, pawns, sWE, theme);
                break;
            case SDL_MOUSEBUTTONDOWN:
                //if (turn == yourTurn) {
                gridXY = gridClick(event.button.x, event.button.y, sWE);
                // }

                if (gridXY != -1) { // si le click est dans le tableau

                    if (gridXY == prevGameValue) {//moves to same spot -> unselects
                        clearMovable(pawns);
                        prevGameValue = -1;
                        d("Deselected");
                        redrawBoard(renderer, board, pawns, sWE, theme);
                        continue;
                    }

                    if (playingGame == 0) {
                        gameValue = katarenga(window, renderer, board, pawns, players, gridXY, gameValue, prevGameValue, turn, round);
                    } else if (playingGame == 1) {
                        gameValue = congress(window, renderer, board, pawns, players, gridXY, gameValue, prevGameValue, turn, round);
                    } else {
                        d("L'islation chez les jeunes, 110%");
                    }

                    if (gameValue == -2) {
                        printf("Winner is player %d!\n", turn + 1);
                        return 1;
                    } else if (gameValue == -1) { // le joeuur a mal joué
                        gameValue = 0;
                        continue;
                    }

                    if (prevGameValue == -1) {
                        prevGameValue = gameValue > 0 ? gameValue : 0;
                        gameValue = 0;
                    } else {
                        prevGameValue = -1;
                        if (gameValue == 0) {
                            turn = switchTurn(turn);
                            printf("Player %d's turn", turn + 1);
                            gridXY = -1;
                            if (!turn) { round++; d("NEW ROUND");}
                        }
                    }
                } else if (turn == yourTurn && playingGame == 0 && canCamp(turn, getSingleCoord(prevGameValue, 0))) { //campClick(event.button.x, event.button.y, sWE)
                    d("I CAN CAMP YOU NIG");
                }

                else {
                    printf("What to do %d;%d\n", event.button.x, event.button.y);
                }
                redrawBoard(renderer, board, pawns, sWE, theme);
                break;
            }
        }
    }

    return 0; // 1 pour continuer a run
};

#endif
