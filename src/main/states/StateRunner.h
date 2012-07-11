#ifndef STATERUNNER_H_
#define STATERUNNER_H_

#include <assert.h>
#include <cstddef>

#include "allegro.h"

#include "../utils/cTimeManager.h"
#include "../managers/RestManager.h"
#include "../domain/Screen.h"
#include "StateBuilder.h"
#include "../include/GameState.h"

class StateRunner {

	public:
		StateRunner(Screen * screen, GameState initialGameState) {
			assert(screen);
			this->stateBuilder = new StateBuilder(screen->getScreenResolution());
			this->runningState = stateBuilder->buildState(initialGameState);
			this->screen = screen;
		}

		~StateRunner() {
			delete runningState;
			delete stateBuilder;
			delete screen;
		}

		void runState() {
			assert(runningState);
			screen->clearBuffer();

			// update time manager state, give CPU slack, update mouse state.
			timeManager->processTime();
			restManager->giveCpuSomeSlackIfNeeded();
			mouse->updateState();

			runningState->manageTime();
			runningState->updateState(this);
			runningState->draw();

			Mouse * mouse = Mouse::getInstance();
			mouse->draw(screen->getBuffer());
			screen->blitScreenBufferToScreen();
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

		Screen * screen;

		RestManager * restManager;

		cTimeManager * timeManager;

		Mouse * mouse;
};

#endif /* STATERUNNER_H_ */
