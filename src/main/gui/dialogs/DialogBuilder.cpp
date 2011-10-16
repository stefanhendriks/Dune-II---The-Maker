#include "allegro.h"
#include "../../include/data/gfxinter.h"

extern DATAFILE * gfxinter;

#include "DialogBuilder.h"


DialogBuilder::DialogBuilder(ScreenResolution * screenResolution) {
	assert(screenResolution);
	this->screenResolution = screenResolution;
}

DialogBuilder::~DialogBuilder() {
	screenResolution = NULL; // note: do not delete screenResolution, as this class is not its owner
}

MainMenuDialog * DialogBuilder::buildMainMenuDialog() {
	int startY = screenResolution->getHeight() * 0.65F;
	int centerOfX = screenResolution->getWidth() / 2;
	int halfOfSizeMainMenu = 65; // total width = 130 pixels
	int startX = centerOfX - halfOfSizeMainMenu;

	Rectangle * dialogRectangle = new Rectangle();
	dialogRectangle->setStartX(startX);
	dialogRectangle->setStartY(startY);
	dialogRectangle->setEndY(startY); // <- this will be calculated below
	dialogRectangle->setEndX(startX); // <- setWidth will take care of this
	dialogRectangle->setWidth(130);

	MainMenuDialog * mainMenuDialog = new MainMenuDialog(dialogRectangle);
	mainMenuDialog->setColors(guiColors.getMenuDarkBorder(), guiColors.getMenuLightBorder(), guiColors.getMenuGrey());

	// start with rectangle within the dialog
	Rectangle * buttonRectangle = new Rectangle(dialogRectangle);
	buttonRectangle->setStartX((dialogRectangle->getStartX() + 1));
	buttonRectangle->setStartY((dialogRectangle->getStartY() + 1));
	buttonRectangle->setEndX(dialogRectangle->getEndX() - 1);
	buttonRectangle->setEndY(dialogRectangle->getEndY() - 1);

	// create them incrementally, so we can use the handy "moveButtonDownExactlyOneButtonHeight" and stack the buttons automatically
	GuiButton * campaignButton = new GuiButton(buttonRectangle, "New Campaign");
	campaignButton->adjustHeightOfButtonToFontHeight();
	campaignButton->setHasBorders(false);
	campaignButton->setEnabled(false);
	mainMenuDialog->setNewCampaignButton(campaignButton);

	GuiButton * skirmishButton = new GuiButton(campaignButton, "Skirmish Game");
	skirmishButton->moveButtonDownExactlyOneButtonHeight();
	skirmishButton->setEnabled(true);
	mainMenuDialog->setSkirmishButton(skirmishButton);

	GuiButton * multiplayerButton = new GuiButton(skirmishButton, "Multiplayer");
	multiplayerButton->setEnabled(false);
	multiplayerButton->moveButtonDownExactlyOneButtonHeight();
	mainMenuDialog->setMultiplayerButton(multiplayerButton);

	GuiButton * loadGameButton = new GuiButton(multiplayerButton, "Load Game");
	loadGameButton->setEnabled(false);
	loadGameButton->moveButtonDownExactlyOneButtonHeight();
	mainMenuDialog->setLoadGameButton(loadGameButton);

	GuiButton * optionsButton = new GuiButton(loadGameButton, "Options");
	optionsButton->moveButtonDownExactlyOneButtonHeight();
	mainMenuDialog->setOptionsButton(optionsButton);

	GuiButton * hallOfFameButton = new GuiButton(optionsButton, "Hall of Fame");
	hallOfFameButton->moveButtonDownExactlyOneButtonHeight();
	mainMenuDialog->setHallOfFameButton(hallOfFameButton);

	GuiButton * exitButton = new GuiButton(hallOfFameButton, "Exit");
	exitButton->moveButtonDownExactlyOneButtonHeight();
	exitButton->setEnabled(true);
	mainMenuDialog->setExitGameButton(exitButton);

	// based on buttons, recalculate height
	mainMenuDialog->recalculateHeight();

	return mainMenuDialog;
}
