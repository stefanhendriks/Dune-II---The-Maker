#include "DialogBuilder.h"

DialogBuilder::DialogBuilder(ScreenResolution * screenResolution) {
	assert(screenResolution);
	this->screenResolution = screenResolution;
}

DialogBuilder::~DialogBuilder() {
	screenResolution = NULL; // note: do not delete screenResolution, as this class is not its owner
}

MainMenuDialog * DialogBuilder::buildMainMenuDialog() {
	int startY = screenResolution->getHeight() * 0.6F;
	int centerOfX = screenResolution->getWidth() / 2;
	int halfOfSizeMainMenu = 65; // total width = 130 pixels
	int startX = centerOfX - halfOfSizeMainMenu;
	int endX = centerOfX + halfOfSizeMainMenu;
	int endY = startY + 142; // 142 pixels height

	Rectangle * dialogRectangle = new Rectangle();
	dialogRectangle->setStartX(startX);
	dialogRectangle->setStartY(startY);
	dialogRectangle->setEndX(endX);
	dialogRectangle->setEndY(endY);

	MainMenuDialog * mainMenuDialog = new MainMenuDialog(dialogRectangle);
	mainMenuDialog->setColors(guiColors.getMenuDarkBorder(), guiColors.getMenuLightBorder(), guiColors.getMenuGrey());

	//mainMenuDialog->setNewCampaignButton()
	Rectangle * buttonRectangle = new Rectangle(dialogRectangle);
	buttonRectangle->setStartX(dialogRectangle->getStartX() + 1);
	buttonRectangle->setStartY(dialogRectangle->getStartY() + 1);
	buttonRectangle->setEndX(dialogRectangle->getEndX() - 1);
	buttonRectangle->setEndY(dialogRectangle->getEndY() - 1);

	GuiButton * button = new GuiButton(buttonRectangle, "Campaign");
	button->adjustHeightOfButtonToFontHeight();
	button->setHasBorders(false);
	mainMenuDialog->setNewCampaignButton(button);

	return mainMenuDialog;
}
