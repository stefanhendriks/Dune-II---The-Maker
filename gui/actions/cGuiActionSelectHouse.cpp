#include "d2tmh.h"

void cGuiActionSelectHouse::execute() {
    game.setNextStateToTransitionTo(GAME_SELECT_HOUSE);
    game.START_FADING_OUT();
}