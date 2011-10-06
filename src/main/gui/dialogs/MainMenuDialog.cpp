/*
 * cMainMenuDialog.cpp
 *
 *  Created on: 1-okt-2011
 *      Author: Stefan
 */
#include "../../include/d2tmh.h"

MainMenuDialog::MainMenuDialog(Rectangle * rect) : GuiShape(rect) {
	newCampaignButton = NULL;
	newSkirmishButton = NULL;
	newMultiplayerButton = NULL;
	loadGameButton = NULL;
	optionsButton = NULL;
	hallfOfFameButton = NULL;
	exitGameButton = NULL;
}

MainMenuDialog::~MainMenuDialog() {
	delete newCampaignButton;
	delete newSkirmishButton;
	delete newMultiplayerButton;
	delete loadGameButton;
	delete optionsButton;
	delete hallfOfFameButton;
	delete exitGameButton;
}
