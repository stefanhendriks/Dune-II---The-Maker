#include "../../include/d2tmh.h"

MainMenuDialog::MainMenuDialog(Rectangle * rect) : GuiShape(rect) {
	newCampaignButton = NULL;
	newSkirmishButton = NULL;
	newMultiplayerButton = NULL;
	loadGameButton = NULL;
	optionsButton = NULL;
	hallOfFameButton = NULL;
	exitGameButton = NULL;
}

MainMenuDialog::~MainMenuDialog() {
	delete newCampaignButton;
	delete newSkirmishButton;
	delete newMultiplayerButton;
	delete loadGameButton;
	delete optionsButton;
	delete hallOfFameButton;
	delete exitGameButton;
}

void MainMenuDialog::draw() {
	GuiShape::draw();
	newCampaignButton->draw();
	newSkirmishButton->draw();
	newMultiplayerButton->draw();
	loadGameButton->draw();
	optionsButton->draw();
	hallOfFameButton->draw();
	exitGameButton->draw();
}

void MainMenuDialog::recalculateHeight() {
	// height is basically the height of all buttons + 2 pixels
	int height = newCampaignButton->getRectangle()->getHeight();
	height += newSkirmishButton->getRectangle()->getHeight();
	height += newMultiplayerButton->getRectangle()->getHeight();
	height += loadGameButton->getRectangle()->getHeight();
	height += optionsButton->getRectangle()->getHeight();
	height += hallOfFameButton->getRectangle()->getHeight();
	height += exitGameButton->getRectangle()->getHeight();
	height += 2;
	rectangle->setHeight(height);
}
