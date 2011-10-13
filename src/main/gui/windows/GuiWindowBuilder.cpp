#include "GuiWindowBuilder.h"

#include "GuiMainMenuWindowInteractionDelegate.h"

GuiWindowBuilder::GuiWindowBuilder() {
}

GuiWindowBuilder::~GuiWindowBuilder() {
}

GuiMainMenuWindow * GuiWindowBuilder::buildMainMenuWindow() {
	GuiMainMenuWindowInteractionDelegate * interactionDelegate = new GuiMainMenuWindowInteractionDelegate();
	GuiMainMenuWindow * instance = new GuiMainMenuWindow(interactionDelegate);

	//MainMenuDialog * mainMenuDialog = new MainMenuDialog();

	// TODO: USE DIALOGBUILDER

	//	instance->setMainMenuDialog(mainMenuDialog);
	delete instance;
}
