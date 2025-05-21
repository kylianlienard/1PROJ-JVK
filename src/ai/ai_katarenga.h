// AI rulesets for katarenga game

#ifndef fileAI_KATARENGA
#define fileAI_KATARENGA

#ifndef fileBOARD
#include "../game/board.c"
#endif

#ifndef fileKATARENGA
#include "../game/katarenga.h"
#endif

#ifndef fileAI
#include "ai.h"
#endif

void ai_katarenga_debug() {
    printf("AI katarenga\n");
    board_use();
    katarenga_use();
    ai_debug();
}

#endif
