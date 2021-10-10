#include "d2tmh.h"

void cProceedButtonCommand::execute(cAbstractMentat &mentat) {
    if (game.isState(GAME_BRIEFING)) {
        // proceed, play mission (it is already loaded before we got here)
        game.setState(GAME_PLAYING);
        drawManager->getMessageDrawer()->initCombatPosition();

        // CENTER MOUSE
        game.getMouse()->positionMouseCursor(game.screen_x / 2, game.screen_y / 2);

        game.FADE_OUT();

        playMusicByType(MUSIC_PEACE);
        return;
    }

    if (game.bSkirmish) {
        if (game.isState(GAME_WINBRIEF) || game.isState(GAME_LOSEBRIEF)) {
            // regardless of winning or losing, always go back to main menu
            game.setState(GAME_SETUPSKIRMISH);
            game.init_skirmish();
            playMusicByType(MUSIC_MENU);
            game.FADE_OUT();
        } else {
            logbook("cProceedButtonCommand pressed, in skirmish mode and state is not WINBRIEF nor LOSEBRIEF!?");
        }
        return;
    }

    // NOT a skirmish game

    // won mission, transition to region selection (Select your next Conquest)
    if (game.isState(GAME_WINBRIEF)) {
        game.setState(GAME_REGION);

        if (game.iMission > 1) {
            selectYourNextConquestState->conquerRegions();
        }

        selectYourNextConquestState->REGION_SETUP_NEXT_MISSION(game.iMission, players[HUMAN].getHouse());

        // PLAY THE MUSIC
        playMusicByType(MUSIC_CONQUEST);

        game.FADE_OUT();
        return;
    }

    // lost mission
    if (game.isState(GAME_LOSEBRIEF)) {
        game.mission_init();
        // lost mission > 1, so we go back to region select
        if (game.iMission > 1)   {
            game.setState(GAME_REGION);

            game.iMission--; // we did not win
            selectYourNextConquestState->REGION_SETUP_LOST_MISSION();

            // PLAY THE MUSIC
            playMusicByType(MUSIC_CONQUEST);
        } else {
            // mission 1 failed, really?..., back to mentat with briefing
            game.setState(GAME_BRIEFING);
            game.prepareMentatForPlayer();
            playMusicByType(MUSIC_BRIEFING);
            mentat.resetSpeak();
        }

        game.FADE_OUT();
        return;
    }
}
