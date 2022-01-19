#include "d2tmh.h"

void cNoButtonCommand::execute(cAbstractMentat&) {
    logbook("cNoButtonCommand::execute()");
    // head back to choose house
    players[HUMAN].setHouse(GENERALHOUSE);
    game.setNextStateToTransitionTo(GAME_SELECT_HOUSE);
    game.initiateFadingOut();
}
