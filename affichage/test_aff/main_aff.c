#include <SDL2/SDL.h>
#include <stdio.h>

#define GRID_SIZE 8

int grid[8][8] = {
    {2, 1, 3, 0, 3, 1, 2, 0},
    {0, 3, 2, 2, 0, 1, 2, 3},
    {3, 0, 1, 1, 2, 0, 3, 1},
    {1, 2, 0, 3, 1, 3, 0, 2},
    {3, 1, 2, 0, 3, 1, 2, 0},
    {0, 1, 2, 3, 0, 1, 2, 3},
    {2, 0, 3, 1, 2, 0, 3, 1},
    {1, 3, 0, 2, 1, 3, 0, 2}
};

void drawGrid(SDL_Renderer* renderer, int size) {
    int cellSize = size / GRID_SIZE;

    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            switch (grid[y][x]) {
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

void handleMouseClick(int mouseX, int mouseY, int size) {
    int cellSize = size / GRID_SIZE;
    int gridX = mouseX / cellSize;
    int gridY = mouseY / cellSize;

    if (gridX < GRID_SIZE && gridY < GRID_SIZE) {
        printf("Clique sur la case [%d][%d] avec valeur : %d\n", gridY, gridX, grid[gridY][gridX]);
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur d'initialisation SDL: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Echiquier Couleur", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 400, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window) {
        printf("Erreur de creation de fenetre: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erreur de creation de renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int width, height;
                SDL_GetWindowSize(window, &width, &height);
                int size = (width < height) ? width : height;
                handleMouseClick(event.button.x, event.button.y, size);
            }
        }

        int width, height;
        SDL_GetWindowSize(window, &width, &height);

        int size = (width < height) ? width : height;

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        drawGrid(renderer, size);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}