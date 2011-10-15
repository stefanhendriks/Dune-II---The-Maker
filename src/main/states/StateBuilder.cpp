#include <assert.h>

#include "StateBuilder.h"
#include "../managers/cInteractionManager.h"
#include "../managers/cCombatInteractionManager.h"
#include "../managers/RestManager.h"
#include "../utils/cTimeManager.h"
#include "../controls/Mouse.h"

StateBuilder::StateBuilder(ScreenResolution * screenResolution) {
	assert(screenResolution);
	this->screenResolution = screenResolution;
	this->guiWindowBuilder = new GuiWindowBuilder(screenResolution);
}

StateBuilder::~StateBuilder() {
	delete guiWindowBuilder;
	screenResolution = NULL; // note: do not delete screenResolution, as this class is not its owner
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

	GuiMainMenuWindow * mainMenuWindow = guiWindowBuilder->buildMainMenuWindow();
	mainMenuState->setGuiWindow(mainMenuWindow);

//	cInteractionManager * interactionManager = new cCombatInteractionManager(player);
//	mainMenuState->setInteractionManager(interactionManager);

	RestManager * restManager = new RestManager(IDEAL_FPS);
	mainMenuState->setRestManager(restManager);

	cTimeManager * timeManager = new cTimeManager();
	mainMenuState->setTimeManager(timeManager);

	mainMenuState->setMouse(Mouse::getInstance());

	return mainMenuState;
}

