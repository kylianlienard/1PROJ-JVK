// All UI componants + UI constants

#ifndef fileUI
#define fileUI

#include <stdio.h>
#include <SDL2/SDL.h>

void ui_debug() {
    printf("UI\n");
}

void showError(const char *msg) {
    printf("ERROR: %s", msg);
}

#endif
