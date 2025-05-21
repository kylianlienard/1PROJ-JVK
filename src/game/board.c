// Manage the game's board data + general game functions

#ifndef fileBOARD
#define fileBOARD

#ifndef fileBOARD_UI
#include "board_ui.h"
#endif

#ifndef fileKATARENGA
#include "katarenga.h"
#endif

#ifndef fileAI
#include "../ai/ai.h"
#endif

#ifndef fileAI_KATARENGA
#include "../ai/ai_katarenga.h"
#endif

void board_debug() {
    printf("Board\n");
    board_ui_debug();
    ai_debug();
    katarenga_debug();
    ai_katarenga_debug();
}

void board_use() {
    printf("Board used\n");
}

#endif
