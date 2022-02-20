#include "cYesButtonCommand.h"

#include "d2tmc.h"
#include "player/cPlayer.h"

void cYesButtonCommand::execute(cAbstractMentat& mentat) {
    logbook("cYesButtonCommand::execute()");
    game.setNextStateToTransitionTo(GAME_BRIEFING);
    game.m_mission = 1; // first mission
    game.m_region  = 1; // and the first "region" so to speak
    game.missionInit();

    players[HUMAN].setHouse(mentat.getHouse());

    game.initiateFadingOut();
}

