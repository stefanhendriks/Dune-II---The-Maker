#include "d2tmh.h"

void cYesButtonCommand::execute(cAbstractMentat& mentat) {
    logbook("cYesButtonCommand::execute()");
    game.setNextStateToTransitionTo(GAME_BRIEFING);
    game.iMission = 1; // first mission
    game.iRegion  = 1; // and the first "region" so to speak
    game.mission_init();

    players[HUMAN].setHouse(mentat.getHouse());

    game.START_FADING_OUT();
}

