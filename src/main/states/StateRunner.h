#ifndef STATERUNNER_H_
#define STATERUNNER_H_

#include <assert.h>
#include <cstddef>

#include "State.h"
#include "StateBuilder.h"
#include "../include/GameState.h"

class StateRunner {
	public:
		StateRunner(StateBuilder * stateBuilder, GameState initialGameState) {
			assert(stateBuilder);
			this->stateBuilder = stateBuilder;
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

	protected:

	private:
		State * runningState;

		StateBuilder * stateBuilder;

};

#endif /* STATERUNNER_H_ */
