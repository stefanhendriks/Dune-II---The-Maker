#ifndef STATEBUILDER_H_
#define STATEBUILDER_H_

#include "State.h"

#include "../include/GameState.h"

class StateBuilder {
	public:
		StateBuilder();
		~StateBuilder();

		State * buildState(GameState gameState);

	protected:

	private:

		State * buildNewMainMenuState();

};

#endif
