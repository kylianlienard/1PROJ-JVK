// All inits (SDL window, time), SDL main, window manager

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#define SDL

#include "./src/core/ui.h"
#include "./src/core/menu.h"
#include "./src/game/board.h"
//char length 32

struct Player {
    char name[20];
    int value;
};

void d(const char *txt) {
    printf("<%s>\n", txt);
}

int main(int argc, char* argv[]) {

    //- Init common values -//

    srand(time(NULL));

    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        printf("WSAStartup failed.");
        return -1;
    }

    Network* network = createNetwork();
    if (!network) {
        printf("ERROR network\n");
        destroyNetworkState(network);
        return 1;
    }

    //- Testing SDL librairies -//

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("ERROR Init SDL: %s\n", SDL_GetError());
        destroyNetworkState(network);
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("App", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 400, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window) {
        printf("ERROR Init Window: %s\n", SDL_GetError());
        SDL_Quit();
        destroyNetworkState(network);
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("ERROR Init Renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        destroyNetworkState(network);
        return -1;
    }

    if (TTF_Init() == -1) {
        printf("Erreur lors de l'initialisation de SDL_ttf: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        WSACleanup();
        return -1;
    }

    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        printf("Erreur lors du chargement de la police: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        destroyNetworkState(network);
    }

    //- Loop -//

    struct MenuParams mp = {1, 0, 'A', 'B'};
    int r = 1;
    int theme = 0;
    setBgTheme(renderer, theme);

    while (r){
        r = menu(window, renderer, font, wsaData, &mp, network);
        if (!r) { break; }

        struct Player* players = (struct Player*)malloc(2 * sizeof(struct Player));
        if (!players) {
            printf("ALLOC ERROR: players\n");
            exit(1);
        }
        for (int plna = 0; plna < 2; plna++) {
            /*printf("Player %d name: ", plna + 1);
            scanf("%32s", players[plna].name);*/
            players[plna].value = 0;
        }

        //printf("BEFORE GAMELOOP: Type %d Game %d P1 %s P2 %s", mp.type, mp.game, players[0].name, players[1].name);
        r = gameLoop(window, renderer, mp.type, mp.game, players, theme);
    }
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    destroyNetworkState(network);

    return 0;
}
