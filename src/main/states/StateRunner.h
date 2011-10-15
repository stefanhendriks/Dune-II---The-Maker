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
			runningState->run(this);
			screenBlitter->blitScreen();
		}

		void transitionToState(GameState gameState);

		bool shouldQuitGame() {
			return runningState->shouldQuitGame();
		}

	protected:

	private:
		State * runningState;

		StateBuilder * stateBuilder;

		ScreenBlitter * screenBlitter;
};

#endif /* STATERUNNER_H_ */
