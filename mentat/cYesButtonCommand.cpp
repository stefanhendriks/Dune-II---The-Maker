#include "d2tmh.h"

void cYesButtonCommand::execute(cAbstractMentat& mentat) {
    logbook("cYesButtonCommand::execute()");
    game.setState(GAME_BRIEFING);
    game.iMission = 1;
    game.iRegion  = 1;
    game.mission_init();

    player[HUMAN].setHouse(mentat.getHouse());

    // play correct mentat music
    playMusicByType(MUSIC_BRIEFING);
    game.FADE_OUT();

    // createAndPrepareMentatForHumanPlayer must be last, as it destroys the old mentat and its associated button (this one)
    // which is fine as long as it is the very last line of this method.
    //
    // Although the 'real' solution would be to have a true state-machine that deals with transitions between
    // states..
    //
    game.createAndPrepareMentatForHumanPlayer();
}

