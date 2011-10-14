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

	Rectangle * rect = new Rectangle();
	rect->setStartX(startX);
	rect->setStartY(startY);
	rect->setEndX(endX);
	rect->setEndY(endY);

	MainMenuDialog * mainMenuDialog = new MainMenuDialog(rect);
	mainMenuDialog->setColors(guiColors.getMenuDarkBorderColor(), guiColors.getMenuLightBorderColor(), guiColors.getMenuGreyColor());

	//mainMenuDialog->setNewCampaignButton()
	return mainMenuDialog;
}
