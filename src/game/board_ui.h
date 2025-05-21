// Creates + manages the UI according to board

#ifndef fileBOARD_UI
#define fileBOARD_UI

#ifndef fileBOARD
#include "board.h"
#endif

#include <SDL2/SDL.h>

void board_ui_debug() {
    printf("UI\n");
    board_use();
}

#endif
