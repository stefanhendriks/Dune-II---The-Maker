#include "d2tmh.h"

cGuiActionToMainMenuState::cGuiActionToMainMenuState() {
    fadeOut = true;
}

void cGuiActionToMainMenuState::execute() {
    game.setNextStateToTransitionTo(GAME_MENU);
    if (fadeOut) {
        game.START_FADING_OUT();
    }
}