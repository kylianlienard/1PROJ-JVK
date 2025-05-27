// Creates + manages the UI according to board

#ifndef fileBOARD_UI
#define fileBOARD_UI

#ifndef fileBOARD
#include "board.h"
#endif

#ifndef SDL
#include <SDL2/SDL.h>
#endif

//- TEMP AVANT GUI -//

char symbl(int value, int boardTab) {
    if (boardTab) {
        switch (value) {
            case 0: return '+';
            case 1: return 'O';
            case 2: return '*';
            case 3: return 'L';
            default: return '?';
        }
    } else {
        if (value == 0) {
            return '.';
        } else {
            return value + '0';
        }
    }
}

void displayBoard(int** board, int boardTab) { // si boardTab == 1 cela affichera les symboles pour "board",  pour "pawns"
    printf(" y ");
    for (int i = 0; i < 8; i++) {
        printf("%d ", i);
    }
    printf("\nx+----------------\n");
    for (int i = 0; i < 8; i++) {
        printf("%d| ", i);
        for (int j = 0; j < 8; j++) {
            printf("%c ", symbl(board[i][j], boardTab));
        }
        printf("\n");
    }
}

void display2Boards(int** board, int** pawns) { /// sera commun
    printf("Board:\n");
    displayBoard(board, 1); // board sera tjrs accomp de 1 et pawns avec 0
    printf("\nPawns:\n");
    displayBoard(pawns, 0);
    printf("\n");
}



void drawBoard(SDL_Renderer* renderer, int** board, int** pawns, int sWE) {
    int y, x;
    int cellSize = sWE / 8;
    int pawnMargin = cellSize / 8;
    int pawnSize = cellSize - (2 * pawnMargin);

    for (y = 0; y < 8; y++) {
        for (x = 0; x < 8; x++) {
            switch (board[y][x]) {
            case 0:
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rouge
                break;
            case 1:
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Bleu
                break;
            case 2:
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Jaune
                break;
            case 3:
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Vert
                break;
            default:
                SDL_SetRenderDrawColor(renderer, 0, 90, 90, 90); // Default
                break;
            }

            SDL_Rect cell = {x * cellSize, y * cellSize, cellSize, cellSize};
            SDL_RenderFillRect(renderer, &cell);

            if (pawns[y][x] >= 3) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
                SDL_Rect playable = {x * cellSize + pawnMargin, y * cellSize + pawnMargin, pawnSize, pawnSize};
                SDL_RenderFillRect(renderer, &playable);
            }

            if (pawns[y][x] == 1 || pawns[y][x] == 2) {
                if (pawns[y][x] == 1) {
                    SDL_SetRenderDrawColor(renderer, 237, 214, 176, 255);
                } else {
                    SDL_SetRenderDrawColor(renderer, 184, 135, 98, 255);
                }

                SDL_Rect pawnRect = {x * cellSize + pawnMargin, y * cellSize + pawnMargin, pawnSize, pawnSize};
                SDL_RenderFillRect(renderer, &pawnRect);
            }
        }
    }
}

//- VRAI -//

int gridClick(int mouseX, int mouseY, int sWE) {
    int cellSize = sWE / 8;
    int gridX = mouseX / cellSize;
    int gridY = mouseY / cellSize;

    if (0 <= gridX && gridX < 8 && 0 <= gridY && gridY < 8) {
        return gridX * 10 + gridY;
    }
    return -1;
}

int getShortestWindowEdge(SDL_Window* window) {
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    return (width < height) ? width : height;
}

#endif
