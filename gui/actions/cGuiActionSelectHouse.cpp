#include "cGuiActionSelectHouse.h"

#include "d2tmc.h"

void cGuiActionSelectHouse::execute() {
    game.setNextStateToTransitionTo(GAME_SELECT_HOUSE);
    game.initiateFadingOut();
}