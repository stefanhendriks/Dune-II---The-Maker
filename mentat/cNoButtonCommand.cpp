#include "cNoButtonCommand.h"

#include "d2tmc.h"
#include "player/cPlayer.h"

void cNoButtonCommand::execute(cAbstractMentat&) {
    logbook("cNoButtonCommand::execute()");
    // head back to choose house
    players[HUMAN].setHouse(GENERALHOUSE);
    game.setNextStateToTransitionTo(GAME_SELECT_HOUSE);
    game.initiateFadingOut();
}
