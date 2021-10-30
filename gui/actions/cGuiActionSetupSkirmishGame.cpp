#include "d2tmh.h"

void cGuiActionSetupSkirmishGame::execute() {
    INI_PRESCAN_SKIRMISH();
    game.init_skirmish();
    game.setNextStateToTransitionTo(GAME_SETUPSKIRMISH);
    game.START_FADING_OUT();
}