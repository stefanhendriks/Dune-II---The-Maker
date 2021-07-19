#include "../include/d2tmh.h"


cTimeManager::cTimeManager() {
    timerUnits = 0;
    timerGlobal = 0;
    timerSecond = 0;

    gameTime = 0;
}

/**
	In case the system locks up, or the computer is on heavy duty. The capping
	makes sure the computer will not cause a chainreaction (getting extremely high timers
	and doing a lot of loops, sucking mure cpu power).

	In most coses this is not nescesary.
**/

void cTimeManager::capTimers() {
    if (timerUnits > 10) {
        if (DEBUGGING) {
            logbook("WARNING: Exeptional high timer; capped");
            timerUnits = 10;
        }
    }

    if (timerGlobal > 40) {
        if (DEBUGGING) {
            logbook("WARNING: Exeptional high timer; capped");
            timerGlobal = 40;
        }
    }

    /* Taking 10 seconds to render a frame? i hope not **/
    if (timerSecond > 10) {
        if (DEBUGGING) {
            logbook("WARNING: Exeptional high timer; capped");
            timerSecond = 10;
        }
    }
}

/**
 * timerseconds timer is called every 1000 ms, try to keep up with that.
 */
void cTimeManager::handleTimerAllegroTimerSeconds() {
    while (timerSecond > 0) {
        gameTime++;

        if (game.isState(GAME_PLAYING)) {
            game.paths_created = 0;

            if (!game.bDisableReinforcements) {
                THINK_REINFORCEMENTS();
            }

            // starports think per second for deployment (if any)
            for (int i = 0; i < MAX_STRUCTURES; i++) {
                cAbstractStructure *pStructure = structure[i];
                if (pStructure && pStructure->isValid()) {
                    if (pStructure->getType() == STARPORT) {
                        ((cStarPort *) pStructure)->think_deploy();
                    }
                }
            }

            for (int i = 0; i < MAX_PLAYERS; i++) {
                if (players[i].getOrderProcesser()) {
                    cOrderProcesser *orderProcesser = players[i].getOrderProcesser();
                    assert(orderProcesser);
                    orderProcesser->think();
                }
            }

        } // game specific stuff


        // Frame Per Second counter
        fps = frame_count;

        // 'auto resting'
        if (fps < IDEAL_FPS) {
            if (iRest > 0) iRest -= 1;
            if (iRest < 0) iRest = 0;
        } else {
            if (iRest < 500) iRest += 1;
            if (iRest > 500) iRest = 500;
        }


        // log the status
        frame_count = 0;

        timerSecond--; // done!
    }

}

/**
 * gametime timer is called every 5 ms, try to keep up with that.
 */
void cTimeManager::handleTimerGameTime() {
    // keep up with time cycles
    while (timerGlobal > 0) {
        if (game.iFadeAction == 1) {
            game.iAlphaScreen -= 2;
            if (game.iAlphaScreen < 0) {
                game.iAlphaScreen = 0;
                game.iFadeAction = 0;
            }
        } else if (game.iFadeAction == 2) {
            game.iAlphaScreen += 2;
            if (game.iAlphaScreen > 255) {
                game.iAlphaScreen = 255;
                game.iFadeAction = 0;
            }
        }

        game.getSoundPlayer()->think();
        game.think_music();
        game.think_mentat();
        game.think_state();


        if (drawManager) {
            if (game.isState(GAME_PLAYING)) {
                if (drawManager->getCreditsDrawer()) {
                    drawManager->getCreditsDrawer()->think();
                }
            }

            // this thing is also in another state
            // TODO: this should be all moved to state specific think/update functions
            if (drawManager->getMessageDrawer()) {
                drawManager->getMessageDrawer()->think();
            }
        }

        // THINKING ONLY WHEN PLAYING / COMBAT
        if (game.isState(GAME_PLAYING)) {
            for (int i = HUMAN; i < MAX_PLAYERS; i++) {
                cPlayer &cPlayer = players[i];
                cSideBar *sidebar = cPlayer.getSideBar();
                if (sidebar) {
                    sidebar->think();
                }
            }

            // structures think
            for (int i = 0; i < MAX_STRUCTURES; i++) {
                cAbstractStructure *pStructure = structure[i];
                if (pStructure == nullptr) continue;
                if (pStructure->isValid()) {
                    pStructure->think();           // think about actions going on
                    pStructure->think_animation(); // think about animating
                    pStructure->think_guard();     // think about 'guarding' the area (turrets only)
                }

                if (pStructure->isDead()) {
                    cStructureFactory::getInstance()->deleteStructureInstance(pStructure);
                }
            }

            for (int i = 0; i < MAX_PLAYERS; i++) {
                cItemBuilder *itemBuilder = players[i].getItemBuilder();
                if (itemBuilder) {
                    itemBuilder->think();
                }
            }


            map.increaseScrollTimer();

            if (map.isTimeToScroll()) {
                map.thinkInteraction();
                map.resetScrollTimer();
            }

            if (game.TIMER_shake > 0) {
                game.TIMER_shake--;
            }

            // units think (move only)
            for (int i = 0; i < MAX_UNITS; i++) {
                cUnit &cUnit = unit[i];
                if (!cUnit.isValid()) continue;

                cUnit.think_position();

                // aircraft
                if (cUnit.isAirbornUnit()) {
                    cUnit.think_move_air();
                }

                // move
                if (cUnit.iAction == ACTION_MOVE || cUnit.iAction == ACTION_CHASE) {
                    cUnit.think_move();
                }

                // guard
                if (cUnit.iAction == ACTION_GUARD) {
                    cUnit.think_guard();
                }

                // move in air
                if (cUnit.iType == ORNITHOPTER &&
                    cUnit.iAction == ACTION_ATTACK) {
                    cUnit.think_move_air(); // keep flying even when attacking
                }
            }

            for (int i = 0; i < MAX_PARTICLES; i++) {
                if (particle[i].isValid()) particle[i].think();
            }

            // when not drawing the options, the game does all it needs to do
            // bullets think
            for (int i = 0; i < MAX_BULLETS; i++) {
                cBullet &cBullet = bullet[i];
                if (cBullet.bAlive) {
                    cBullet.think();
                }
            }
        }

        if (game.isState(GAME_WINNING)) {

        }

        game.think_fading();

        timerGlobal--;
    }
}

/**
 * units timer is called every 100 ms, try to keep up with that.
 */
void cTimeManager::handleTimerUnits() {
    while (timerUnits > 0) {
        if (game.isState(GAME_PLAYING)) {
            // units think
            for (int i = 0; i < MAX_UNITS; i++) {
                cUnit &cUnit = unit[i];
                if (cUnit.isValid()) {
                    cUnit.think();

                    // Think attack style
                    if (cUnit.iAction == ACTION_ATTACK) {
                        cUnit.think_attack();
                    }
                }
            }

            map.think_minimap();

            for (int i = 0; i < MAX_PLAYERS; i++) {
                players[i].think();
            }

        }

        timerUnits--;
    }
}

/**
	This function is called by the game class in the run() function.

	It is important that this function is called first, as it will make sure
	the situation is set as it should be (since time passed between the previous
	and the current frame), therefore any think() function is relying on the data
	that eventually is set by this function.

    Do note this, is basically just an "update" call, but takes amount of update cycles into account, so we make sure
    we do not skip or get sloppy with update loops. Although, the entire 'update' in this game is a bit hairy, you would
    expect this function to be called by the main update function. Heck, even per state this would be different - which
    is now not the case.
*/
void cTimeManager::processTime() {
#ifdef ALLEGRO_H
    syncFromAllegroTimers();
#endif

    capTimers();
    handleTimerAllegroTimerSeconds();
    handleTimerUnits();
    handleTimerGameTime();

#ifdef ALLEGRO_H
    syncAllegroTimers();
#endif
}

void cTimeManager::increaseTimerForFPS() {
    timerSecond++;
}

void cTimeManager::increaseTimerForUnits() {
    timerUnits++;
}

void cTimeManager::increaseTimerGlobal() {
    timerGlobal++;
}

/** allegro specific routine **/
void cTimeManager::syncFromAllegroTimers() {
    timerSecond = allegro_timerSecond;
    timerGlobal = allegro_timerGlobal;
    timerUnits = allegro_timerUnits;
}

/** allegro specific routine **/
void cTimeManager::syncAllegroTimers() {
    allegro_timerSecond = timerSecond;
    allegro_timerGlobal = timerGlobal;
    allegro_timerUnits = timerUnits;
}
