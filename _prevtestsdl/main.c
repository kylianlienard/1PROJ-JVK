#include <stdio.h>
#include <SDL2/SDL.h>

int main(int argc, char* argv[]){
    SDL_Init( SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("HELLO", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600,SDL_WINDOW_ALLOW_HIGHDPI);

    if (window == NULL){
        printf("Erreur : window pas cree");
    };
    SDL_Event windowEvent;
    while (1) {
        if (SDL_PollEvent (&windowEvent)){
            if (SDL_Quit == windowEvent.type){
                break;
            };
        };
    };
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

//gcc SDL2/