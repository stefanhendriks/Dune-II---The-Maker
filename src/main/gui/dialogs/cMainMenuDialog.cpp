/*
 * cMainMenuDialog.cpp
 *
 *  Created on: 1-okt-2011
 *      Author: Stefan
 */
#include "../../include/d2tmh.h"

cMainMenuDialog::cMainMenuDialog(cRectangle * rect) : cGuiShape(rect) {
	newCampaignButton = NULL;
	newSkirmishButton = NULL;
	newMultiplayerButton = NULL;
	loadGameButton = NULL;
	optionsButton = NULL;
	hallfOfFameButton = NULL;
	exitGameButton = NULL;
}

cMainMenuDialog::~cMainMenuDialog() {
	delete newCampaignButton;
	delete newSkirmishButton;
	delete newMultiplayerButton;
	delete loadGameButton;
	delete optionsButton;
	delete hallfOfFameButton;
	delete exitGameButton;
}
