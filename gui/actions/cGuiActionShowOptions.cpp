#include "d2tmh.h"

void cGuiActionShowOptions::execute() {
    game.setNextStateToTransitionTo(GAME_OPTIONS);
}