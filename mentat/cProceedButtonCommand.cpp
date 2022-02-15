#include "d2tmh.h"

#include "managers/cDrawManager.h"

void cProceedButtonCommand::execute(cAbstractMentat &mentat) {
    if (game.isState(GAME_BRIEFING)) {
        // proceed, play mission (it is already loaded before we got here)
        game.setNextStateToTransitionTo(GAME_PLAYING);
        drawManager->getMessageDrawer()->initCombatPosition();

        // CENTER MOUSE
        game.getMouse()->positionMouseCursor(game.m_screenX / 2, game.m_screenY / 2);

        game.initiateFadingOut();

        game.playMusicByType(MUSIC_PEACE);
        return;
    }

    if (game.m_skirmish) {
        if (game.isState(GAME_WINBRIEF) || game.isState(GAME_LOSEBRIEF)) {
            // regardless of drawStateWinning or drawStateLosing, always go back to main menu
            game.setNextStateToTransitionTo(GAME_SETUPSKIRMISH);
            game.initSkirmish();
            game.playMusicByType(MUSIC_MENU);
            game.initiateFadingOut();
        } else {
            logbook("cProceedButtonCommand pressed, in skirmish mode and state is not WINBRIEF nor LOSEBRIEF!?");
        }
        return;
    }

    // NOT a skirmish game

    // won mission, transition to region selection (Select your next Conquest)
    if (game.isState(GAME_WINBRIEF)) {
        game.setNextStateToTransitionTo(GAME_REGION);

        // PLAY THE MUSIC
        game.playMusicByType(MUSIC_CONQUEST);

        game.initiateFadingOut();
        return;
    }

    // lost mission
    if (game.isState(GAME_LOSEBRIEF)) {
        game.missionInit();
        // lost mission > 1, so we go back to region select
        if (game.m_mission > 1)   {
            game.setNextStateToTransitionTo(GAME_REGION);

            game.m_mission--; // we did not win

            // PLAY THE MUSIC
            game.playMusicByType(MUSIC_CONQUEST);
        } else {
            // mission 1 failed, really?..., back to mentat with briefing
            game.setNextStateToTransitionTo(GAME_BRIEFING);
            game.prepareMentatForPlayer();
            game.playMusicByType(MUSIC_BRIEFING);
            mentat.resetSpeak();
        }

        game.initiateFadingOut();
        return;
    }
}
