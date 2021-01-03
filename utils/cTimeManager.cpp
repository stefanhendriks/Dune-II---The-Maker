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
		if (DEBUGGING)	{
			logbook("WARNING: Exeptional high timer; capped");
			timerGlobal = 40;
		}
	}

	/* Taking 10 seconds to render a frame? i hope not **/
	if (timerSecond > 10) {
		if (DEBUGGING)	{
			logbook("WARNING: Exeptional high timer; capped");
			timerSecond = 10;
		}
	}
}

void cTimeManager::handleTimerFPS() {
    while (timerSecond > 0) {
		gameTime++;

        if (game.isState(GAME_PLAYING)) {
			game.paths_created = 0;

			THINK_REINFORCEMENTS();

			// starports think per second for deployment (if any)
			for (int i =0; i < MAX_STRUCTURES; i++) {
                cAbstractStructure *pStructure = structure[i];
                if (pStructure && pStructure->isValid()) {
					if (pStructure->getType() == STARPORT) {
						((cStarPort *) pStructure)->think_deploy();
					}
				}
			}

			for (int i = 0; i < MAX_PLAYERS; i++) {
				if (player[i].getOrderProcesser()) {
					cOrderProcesser * orderProcesser = player[i].getOrderProcesser();
					assert(orderProcesser);
					orderProcesser->think();
				}
			}

        } // game specific stuff


		// Frame Per Second counter
		fps = frame_count;

		// 'auto resting'
		if (fps < IDEAL_FPS) {
			if (iRest > 0) iRest-=1;
			if (iRest < 0) iRest=0;
		} else {
			if (iRest < 500) iRest+=1;
			if (iRest > 500) iRest=500;
		}


		// log the status
		frame_count = 0;

		timerSecond--; // done!
	}

}

void cTimeManager::handleTimerGlobal() {
	// keep up with time cycles
	while (timerGlobal > 0) {
		if (game.iFadeAction == 1) {
			game.iAlphaScreen-=2;
			if (game.iAlphaScreen < 0) {
				game.iAlphaScreen = 0;
				game.iFadeAction=0;
			}
		} else if (game.iFadeAction == 2) {
			game.iAlphaScreen+=2;
			if (game.iAlphaScreen > 255) {
				game.iAlphaScreen = 255;
				game.iFadeAction=0;
			}
		}

		game.getSoundPlayer()->think();
		game.think_music();
		game.think_mentat();
		game.think_state();


		if (drawManager) {
			if (drawManager->getCreditsDrawer()) {
				drawManager->getCreditsDrawer()->think();
			}
			if (drawManager->getMessageDrawer()) {
				drawManager->getMessageDrawer()->think();
			}
		}

		for (int i = HUMAN; i < MAX_PLAYERS; i++) {
            cPlayer &cPlayer = player[i];
            cSideBar *sidebar = cPlayer.getSideBar();
            if (sidebar) {
				sidebar->think();
			}
		}

		// THINKING ONLY WHEN PLAYING / COMBAT
		if (game.isState(GAME_PLAYING)) {

			// structures think
			for (int i=0; i < MAX_STRUCTURES; i++) {
                cAbstractStructure *pStructure = structure[i];
                if (pStructure && pStructure->isValid()) {
                    pStructure->think();           // think about actions going on
                    pStructure->think_animation(); // think about animating
                    pStructure->think_guard();     // think about 'guarding' the area (turrets only)
                }
            }

				// DO NOT THINK FOR HUMAN PLAYER (== 0)
				for (int i=0; i < MAX_PLAYERS; i++) {
					aiplayer[i].think();
				}

				for (int i = 0; i < MAX_PLAYERS; i++) {
					cItemBuilder *itemBuilder = player[i].getItemBuilder();
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
				for (int i=0; i < MAX_UNITS; i++) {
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

                for (int i=0; i < MAX_PARTICLES; i++) {
                    if (particle[i].isValid()) particle[i].think();
                }

                // when not drawing the options, the game does all it needs to do
                // bullets think
                for (int i=0; i < MAX_BULLETS; i++) {
                    cBullet &cBullet = bullet[i];
                    if (cBullet.bAlive) {
                        cBullet.think();
                    }
                }
		}

		if (game.isState(GAME_WINNING))
		{

		}

		game.think_fading();

		timerGlobal--;
  }
}

void cTimeManager::handleTimerUnits() {

	while (timerUnits > 0)
	{
		if (game.isState(GAME_PLAYING))
		{
			// units think
			for (int i=0; i < MAX_UNITS; i++)
				if (unit[i].isValid())
				{
					unit[i].think();

					// Think attack style
					if (unit[i].iAction == ACTION_ATTACK)
						unit[i].think_attack();
				}
			map.think_minimap();
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
*/
void cTimeManager::processTime() {
#ifdef ALLEGRO_H
	syncFromAllegroTimers();
#endif

	capTimers();
	handleTimerFPS();
	handleTimerUnits();
	handleTimerGlobal();

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
	allegro_timerSecond  = timerSecond;
	allegro_timerGlobal  = timerGlobal;
	allegro_timerUnits   = timerUnits;
}
