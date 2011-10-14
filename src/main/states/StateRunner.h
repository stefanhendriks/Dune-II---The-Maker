#ifndef STATERUNNER_H_
#define STATERUNNER_H_

#include <assert.h>
#include <cstddef>

#include "StateBuilder.h"
#include "../include/GameState.h"

class StateRunner {
	public:
		StateRunner(ScreenResolution * screenResolution, GameState initialGameState) {
			assert(screenResolution);
			this->stateBuilder = new StateBuilder(screenResolution);
			this->runningState = stateBuilder->buildState(initialGameState);
		}

		~StateRunner() {
			delete runningState;
			delete stateBuilder;
		}

		void runState() {
			assert(runningState);
			runningState->run(this);
		}

		void transitionToState(GameState gameState);

		bool shouldQuitGame() {
			return runningState->shouldQuitGame();
		}

	protected:

	private:
		State * runningState;

		StateBuilder * stateBuilder;
};

#endif /* STATERUNNER_H_ */
