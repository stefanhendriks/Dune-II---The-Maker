#ifndef STATERUNNER_H_
#define STATERUNNER_H_

#include <assert.h>
#include <cstddef>

#include "allegro.h"

#include "../utils/ScreenBlitter.h"
#include "StateBuilder.h"
#include "../include/GameState.h"

class StateRunner {
	public:
		StateRunner(ScreenResolution * screenResolution, BITMAP * bufferScreen, GameState initialGameState) {
			assert(screenResolution);
			this->stateBuilder = new StateBuilder(screenResolution);
			this->screenBlitter = new ScreenBlitter(screenResolution, bufferScreen);
			this->runningState = stateBuilder->buildState(initialGameState);
		}

		~StateRunner() {
			delete runningState;
			delete stateBuilder;
			delete screenBlitter;
		}

		void runState() {
			assert(runningState);
			screenBlitter->clearBuffer();

			// update time manager state, give CPU slack, update mouse state.
			timeManager->processTime();
			restManager->giveCpuSomeSlackIfNeeded();
			mouse->updateState();

			runningState->manageTime();
			runningState->updateState(this);
			runningState->draw();

			screenBlitter->blitMouseToScreenBuffer(); // <-- TEMPORARILY!?
			screenBlitter->blitScreenBufferToScreen();
		}

		void transitionToState(GameState gameState);

		bool shouldQuitGame() {
			return runningState->shouldQuitGame();
		}

		void setTimeManager(cTimeManager * timeManager) {
			assert(timeManager);
			this->timeManager = timeManager;
		}

		void setRestManager(RestManager * restManager) {
			assert(restManager);
			this->restManager = restManager;
		}

		void setMouse(Mouse * mouse) {
			assert(mouse);
			this->mouse = mouse;
		}
	protected:

	private:
		State * runningState;

		StateBuilder * stateBuilder;

		ScreenBlitter * screenBlitter;

		RestManager * restManager;

		cTimeManager * timeManager;

		Mouse * mouse;



};

#endif /* STATERUNNER_H_ */
