#include "cGuiActionSetupSkirmishGame.h"

#include "d2tmc.h"
#include "ini.h"

void cGuiActionSetupSkirmishGame::execute() {
    game.prescanSkirmish();
    game.initSkirmish();
    game.setNextStateToTransitionTo(GAME_SETUPSKIRMISH);
    game.initiateFadingOut();
}