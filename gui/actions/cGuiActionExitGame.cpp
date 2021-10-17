#include "d2tmh.h"

void cGuiActionExitGame::execute() {
    game.bPlaying = false;
    game.START_FADING_OUT();
}