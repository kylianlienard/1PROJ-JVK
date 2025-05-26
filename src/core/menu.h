// Menu UI

#ifndef fileMENU
#define fileMENU

#ifndef fileUI
#include "ui.h"
#endif

#include "online.h"

#include <stdio.h>
#include <SDL2/SDL.h>

void menu_debug() {
    printf("Menu\n");
    ui_debug();
    online_debug();
}

#endif
