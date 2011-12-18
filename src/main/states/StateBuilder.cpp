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

MainMenuState * StateBuilder::buildNewMainMenuState() {
	MainMenuState * mainMenuState = new MainMenuState();

	GuiMainMenuWindow * mainMenuWindow = guiWindowBuilder->buildMainMenuWindow();
	mainMenuState->setGuiWindow(mainMenuWindow);

//	cInteractionManager * interactionManager = new cCombatInteractionManager(player);
//	mainMenuState->setInteractionManager(interactionManager);

	return mainMenuState;
}

