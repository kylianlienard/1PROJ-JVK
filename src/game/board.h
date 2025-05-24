// Manage the game's board data + general game functions

#ifndef fileBOARD
#define fileBOARD

#ifndef fileBOARD_UI
#include "board_ui.h"
#endif

#ifndef fileAI
#include "../ai/ai.h"
#endif

short selectPawn(short** pawns, short turn, short x, short y);
short canMove(short** pawns, short x, short y);
int gameLoop(SDL_Window*, SDL_Renderer*);
unsigned short randint(short maxi) { return rand() % maxi; }
unsigned short rand8() { return rand() % 8; }

#ifndef fileKATARENGA
#include "katarenga.h"
#endif

/*#ifndef fileAI_KATARENGA
#include "../ai/ai_katarenga.h"
#endif*/

/*void board_debug() {
    d("Board\n");
    board_ui_debug();
    ai_debug();
    katarenga_debug();
    ai_katarenga_debug();
}*/

void board_use() {
    d("Board used\n");
}

//- Constants -\\

const short RED_DIRS[4][2] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}};
const short BLUE_DIRS[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
const short GREEN_DIRS[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
const short YELLOW_DIRS[8][2] = {{-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}, {1, -2}, {2, -1}, {2, 1}, {1, 2}};

struct Player { // temp
    char name[20];
    short value;
};

//- Board generation -\\

short** init8by8board() {
    short** eight = (short**)malloc(8 * sizeof(short*));
    if (!eight) {
        d("ALLOC ERROR: eight\n");
        exit(1);
    }

    for (int i = 0; i < 8; i++) {
        eight[i] = (short*)malloc(8 * sizeof(short));
        if (!eight[i]) {
            d("ALLOC ERROR: eight[i]\n");

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

void shuffle(short *arr, short length) {
    short i, j, tmp;
    for (i = length - 1; i > 0; i--) {
        j = randint(i);
        tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
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

void generateBoard(short** board) {
    const short quadrants[4][4][4] = {
        {{2, 1, 3, 0}, {0, 3, 2, 2}, {3, 0, 1, 1}, {1, 2, 0, 3}},
        {{3, 1, 2, 0}, {0, 1, 2, 3}, {2, 0, 3, 1}, {1, 3, 0, 2}},
        {{3, 1, 0, 2}, {1, 3, 1, 0}, {2, 0, 2, 3}, {0, 3, 2, 1}},
        {{2, 1, 3, 0}, {0, 1, 2, 2}, {1, 3, 0, 3}, {3, 0, 2, 1}}
    };

    short** quad = (short**)malloc(4 * sizeof(short*));

    if (!quad) {
        d("ALLOC ERROR: quad\n");
        exit(1);
    }

    for (int i = 0; i < 4; i++) {
        quad[i] = (short*)malloc(4 * sizeof(short));
        if (!quad[i]) {
            d("ALLOC ERROR: quad[i]\n");

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

//- Game functions -\\

short correctCoord(short x, short y) {
    return 0 <= x && x < 8 && 0 <= y && y < 8;
}

short selectPawn(short** pawns, short turn, short x, short y) { // Erreurs + specifiques
    return pawns[x][y] == turn + 1;
}

short canMove(short** pawns, short x, short y) { return pawns[x][y] > 2; };

void setMovable(short** pawns, short x, short y) {
    pawns[x][y] += 3;
    // update GUI
}

short canPlace(short** pawns, short turn, short x, short y, short round) {
    if (pawns[x][y] == 0) { return 1; }
    if (pawns[x][y] == 1 - (turn + 1) && round > 1) { return 1; }
    return 0;
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

void clearMovable(short** pawns) {
    short i, j;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            if (pawns[i][j] > 2) {
                pawns[i][j] -= 3;
            }
        }
    }
}

int gameLoop(SDL_Window* window, SDL_Renderer* renderer) {
    SDL_Event event;
    short playingGame = 0; // which game
    short gameState = 0; // 0 rien, 1 un pion est select, -1 si fin. ne change pas si isolation
    short prevGameState = 0;
    short turn = 0;
    short round = 0;
    short sWE = getShortestWindowEdge(window);
    short clickXY = 0;

    struct Player* players = (struct Player*)malloc(2 * sizeof(struct Player));
    if (!players) {
        d("ALLOC ERROR: players\n");
        exit(1);
    }
    for (short plna = 0; plna < 2; plna++) {
        //printf("Player %d name: ", plna + 1);
        //scanf("%19s", players[plna].name);
        players[plna].value = 0;
    }

    short** board = init8by8board();
    short** pawns = init8by8board();

    generateBoard(board);
    drawBoard(renderer, board, sWE);

    while (1) {
        while (SDL_PollEvent(&event)) { // bouge souris
            switch(event.type){
            case SDL_QUIT:
                return 0;
            case SDL_WINDOWEVENT_RESIZED:
                d("Window resized");
                sWE = getShortestWindowEdge(window);
            case SDL_MOUSEBUTTONDOWN:
                clickXY = gridClick(event.button.x, event.button.y, sWE);

                if (clickXY != -1) { // si le click est dans le tableau
                    printf("board clicked in %d;%d\n", clickXY % 10, (clickXY - clickXY % 10) / 10);

                    if (playingGame == 0) {
                        gameState = katarenga(window, renderer, board, pawns, players, clickXY % 10, (clickXY - clickXY % 10) / 10, gameState, turn, round);
                    } else if (playingGame == 1) {
                        d("Conge");
                    } else {
                        d("L'islation chez les jeunes, 110%");
                    }

                    if (prevGameState == gameState) { // le joeuur a mal joué
                        continue;
                    }

                    d("Next round\n");

                    if (gameState == -1) {
                        printf("Winner is player %d!\n", turn + 1);
                    }

                    //drawBoard(renderer, board, sWE);

                    printf("Player %d's turn", turn + 1);
                }
            }
        }

    }

    return 0; // 1 pour continuer a run
}

#endif
