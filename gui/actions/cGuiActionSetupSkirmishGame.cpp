#include "cGuiActionSetupSkirmishGame.h"

#include "d2tmc.h"
#include "ini.h"

void cGuiActionSetupSkirmishGame::execute() {
    game.loadSkirmishMaps();
    game.initSkirmish();
    game.setNextStateToTransitionTo(GAME_SETUPSKIRMISH);
    game.initiateFadingOut();
}