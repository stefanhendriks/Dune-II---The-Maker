#include "../../include/d2tmh.h"

cGuiShapeFactory::cGuiShapeFactory(cScreenResolution * theScreenResolution) {
	assert(theScreenResolution);
	defaultGreyColorInner  = makecol(176, 176, 196);
	defaultGreyColorLighterBorder = makecol(252, 252, 252);
	defaultGreyColorDarkerBorder = makecol(84, 84, 120);
	screenResolution = theScreenResolution;
}

cGuiShapeFactory::~cGuiShapeFactory() {
}

cGuiButton * cGuiShapeFactory::createGrayButton(cRectangle * rect, std::string theLabel) {
	cGuiButton * button = new cGuiButton(rect, theLabel);
	button->setColors(defaultGreyColorDarkerBorder, defaultGreyColorLighterBorder, defaultGreyColorInner);
	return button;
}

cGuiButton * cGuiShapeFactory::createButtonWithBitmap(int x, int y, BITMAP * bmp, std::string theLabel) {
	assert(bmp);
	cGuiButton * button = new cGuiButton(x, y, bmp->w, bmp->h, theLabel);
	button->setBitmap(bmp);
	return button;
}

cMainMenuDialog * cGuiShapeFactory::createMainMenuDialog() {
	int startY = screenResolution->getHeight() * 0.6F;
	int centerOfX = screenResolution->getWidth() / 2;
	int halfOfSizeMainMenu = 65; // total width = 130 pixels
	int startX = centerOfX - halfOfSizeMainMenu;
	int endX = centerOfX + halfOfSizeMainMenu;
	int endY = startY + 142; // 142 pixels height

	cRectangle * rect = new cRectangle();
	rect->setStartX(startX);
	rect->setStartY(startY);
	rect->setEndX(endX);
	rect->setEndY(endY);

	cMainMenuDialog * mainMenuDialog = new cMainMenuDialog(rect);
	mainMenuDialog->setColors(defaultGreyColorDarkerBorder, defaultGreyColorLighterBorder, defaultGreyColorInner);

	//mainMenuDialog->setNewCampaignButton()
	return mainMenuDialog;
}
