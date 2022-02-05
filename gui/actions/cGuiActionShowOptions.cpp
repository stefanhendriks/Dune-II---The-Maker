#include "cGuiActionShowOptions.h"

#include "d2tmc.h"

void cGuiActionShowOptions::execute() {
    game.setNextStateToTransitionTo(GAME_OPTIONS);
}