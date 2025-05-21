#ifndef SDL.h
#include <SDL2/SDL.h>
#endif

#define boarduifile
#define GRID_SIZE 8

short getMinDimension(SDL_Window* window) {
    short width, height;
    SDL_GetWindowSize(window, &width, &height);
    return (width < height) ? width : height;
}

void drawBoard(SDL_Renderer* renderer, short** board, short newSize) {

    int cellSize = newSize / 8;

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
            }
            SDL_Rect cell = {x * cellSize, y * cellSize, cellSize, cellSize};
            SDL_RenderFillRect(renderer, &cell);
        }
    }
}

short gridClick(short mouseX, short mouseY, short newSize) {
    int cellSize = newSize / 8;
    int gridX = mouseX / cellSize;
    int gridY = mouseY / cellSize;

    if (0 <= gridX && gridX < 8 && 0 <= gridY && gridY < 8) {
        return gridX * 10 + gridY;
    }
    return 0;
}

void showError(const char *text) {
    printf("%s\n", text);
}
