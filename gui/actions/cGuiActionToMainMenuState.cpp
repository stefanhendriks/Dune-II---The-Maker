#include "d2tmh.h"

void cGuiActionToMainMenuState::execute() {
    game.setNextStateToTransitionTo(GAME_MENU);
    game.START_FADING_OUT();
}