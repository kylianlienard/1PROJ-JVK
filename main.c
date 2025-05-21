// All inits (SDL window, time), SDL main, window manager

#include <stdio.h>
//#include <stdlib.h>
//#include <stdint.h>
#include <string.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
//#include <SDL2/SDL_mixer.h>

#include "./src/core/ui.h"
#include "./src/core/menu.h"
//#include "./src/game/board.c"

int debug(int argc, char *argv[]) {
    printf("Main init\n");
    ui_debug();
    printf("--------------------------\n");
    menu_debug();
    printf("--------------------------\n");
    board_debug();
    return 0;
}

#define GRID_SIZE 8
#define CHAR_LENGTH 32

struct Player {
    char name[20];
    short value;
};

int main(int argc, char* argv[]);
short click(short x, short y) {}

//#include "./src/game/katarenga.h"

int main(int argc, char* argv[]) {

    //- Testing SDL librairies -\\

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur d'initialisation SDL: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("App", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 400, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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

    // more tests

    //- Init common values -\\

    srand(time(NULL));

    struct Player* players = (struct Player*)malloc(2 * sizeof(struct Player));
    if (!players) {
        printf("ALLOC ERROR: players\n");
        exit(1);
    }

    for (short plna = 0; plna < 2; plna++) {
        //printf("Player %d name: ", plna + 1);
        //scanf("%19s", players[plna].name);
        players[plna].value = 0;
    }

    //- Loop -\\

    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) { // tous event
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                click(event.button.x, event.button.y);
            }
            //printf("je suis pollevent ");
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

// Fonctionne SDL
 /**void showMainMenu(SDL_Renderer* renderer, int width, int height, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Fond gris clair
    SDL_RenderClear(renderer);

    // Dessiner un bouton (bleu)
    SDL_SetRenderDrawColor(renderer, 50, 150, 255, 255);
    SDL_Rect button = {width / 4, height / 3, width / 2, height / 5};
    SDL_RenderFillRect(renderer, &button);

    // Bordure du bouton (noir)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &button);

    // Affichage du texte "Jouer"
    SDL_Color textColor = {255, 255, 255, 255}; // Blanc
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Jouer", textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    // Position du texte au centre du bouton
    SDL_Rect textRect = {button.x + button.w / 4, button.y + button.h / 4, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) {
        printf("Erreur d'initialisation SDL ou SDL_ttf: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Page d'Accueil", SDL_WINDOWPOS_CENTERED,
SDL_WINDOWPOS_CENTERED, 400, 400, SDL_WINDOW_SHOWN);
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

    // Charger une police
    TTF_Font* font = TTF_OpenFont("./src/ressource/font/arial.ttf", 24);
    if (!font) {
        printf("Erreur de chargement de la police: %s\n", TTF_GetError());
        return -1;
    }

    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;

                // Vérifie si le clic est dans le bouton
                if (mouseX > 100 && mouseX < 300 && mouseY > 133 && mouseY < 266) {
                    printf("Bouton cliqué ! Lancement de la grille...\n");
                    // Ici, tu peux appeler ta fonction principale pour la grille
                }
            }
        }

        int width, height;
        SDL_GetWindowSize(window, &width, &height);

        showMainMenu(renderer, width, height, font);
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}**/
