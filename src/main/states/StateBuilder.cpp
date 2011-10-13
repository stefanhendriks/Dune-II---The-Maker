#include <assert.h>

#include "StateBuilder.h"

StateBuilder::StateBuilder() {
//TODO: inject dependencies (builders)
}

StateBuilder::~StateBuilder() {
}


State * StateBuilder::buildState(GameState gameState) {
	switch(gameState) {
		case MAINMENU:
			return buildNewMainMenuState();
		default:
			assert(false);
	}
}

State * StateBuilder::buildNewMainMenuState() {
	State * mainMenuState = new State();

	// TODO:
	// set stuff in the main menu, which need to be built by their own builders as well...

	return mainMenuState;
}

