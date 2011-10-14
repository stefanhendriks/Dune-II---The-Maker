#include "GuiWindowBuilder.h"

#include "GuiMainMenuWindowInteractionDelegate.h"

GuiWindowBuilder::GuiWindowBuilder(ScreenResolution * screenResolution) {
	assert(screenResolution);
	this->screenResolution = screenResolution;

	// create instances of dependencies
	this->dialogBuilder = new DialogBuilder(screenResolution);
}

GuiWindowBuilder::~GuiWindowBuilder() {
	delete dialogBuilder;
	screenResolution = NULL; // note: do not delete screenResolution, as this class is not its owner
}

GuiMainMenuWindow * GuiWindowBuilder::buildMainMenuWindow() {
	GuiMainMenuWindowInteractionDelegate * interactionDelegate = new GuiMainMenuWindowInteractionDelegate();
	GuiMainMenuWindow * instance = new GuiMainMenuWindow(interactionDelegate);

	MainMenuDialog * mainMenuDialog = dialogBuilder->buildMainMenuDialog();
	instance->setMainMenuDialog(mainMenuDialog);

	return instance;
}
