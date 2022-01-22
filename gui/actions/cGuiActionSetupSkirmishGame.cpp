#include "d2tmh.h"

void cGuiActionSetupSkirmishGame::execute() {
    INI_PRESCAN_SKIRMISH();
    game.initSkirmish();
    game.setNextStateToTransitionTo(GAME_SETUPSKIRMISH);
    game.initiateFadingOut();
}