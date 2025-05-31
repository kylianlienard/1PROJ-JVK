// All UI componants + UI constants

#ifndef fileUI
#define fileUI

#include <stdio.h>
#include <SDL2/SDL.h>

void ui_debug() {
    printf("UI\n");
}

void showError(const char *msg) {
    printf("ERROR: %s\n", msg);
}

void setBgTheme(SDL_Renderer* renderer, int theme) {
    if (theme == 1) {
        SDL_SetRenderDrawColor(renderer, 48, 46, 43, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }
}

#endif
