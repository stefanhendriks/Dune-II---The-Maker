#include "cGuiActionSetupSkirmishGame.h"

#include "d2tmc.h"
#include "ini.h"

void cGuiActionSetupSkirmishGame::execute() {
    INI_PRESCAN_SKIRMISH();
    game.initSkirmish();
    game.setNextStateToTransitionTo(GAME_SETUPSKIRMISH);
    game.initiateFadingOut();
}