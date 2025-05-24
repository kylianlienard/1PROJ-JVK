// Creates + manages the UI according to board

#ifndef fileBOARD_UI
#define fileBOARD_UI

#ifndef fileBOARD
#include "board.h"
#endif

#ifndef SDL
#include <SDL2/SDL.h>
#endif

void board_ui_debug() {
    printf("UI\n");
    board_use();
}

void drawBoard(SDL_Renderer* renderer, short** board, short sWE) {

    int cellSize = sWE / 8;

    d("Drawing grid");

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
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
                default: SDL_SetRenderDrawColor(renderer, 0, 90, 90, 90); break;
            }
            SDL_Rect cell = {x * cellSize, y * cellSize, cellSize, cellSize};
            SDL_RenderFillRect(renderer, &cell);
            d("Rectangle created");
        }
    }
}

short gridClick(short mouseX, short mouseY, short sWE) {
    int cellSize = sWE / 8;
    int gridX = mouseX / cellSize;
    int gridY = mouseY / cellSize;

    if (0 <= gridX && gridX < 8 && 0 <= gridY && gridY < 8) {
        return gridX * 10 + gridY;
    }
    return -1;
}

short getShortestWindowEdge(SDL_Window* window) {
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    return (width < height) ? width : height;
}

#endif
