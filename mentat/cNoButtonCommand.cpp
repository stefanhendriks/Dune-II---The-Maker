#include "d2tmh.h"

void cNoButtonCommand::execute(cAbstractMentat& mentat) {
    logbook("cNoButtonCommand::execute()");
    // head back to choose house
    player[HUMAN].setHouse(GENERALHOUSE);
    game.setState(GAME_SELECT_HOUSE);
    game.FADE_OUT();
}
