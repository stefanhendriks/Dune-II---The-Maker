#include "../../include/d2tmh.h"

GuiShapeFactory::GuiShapeFactory(ScreenResolution * theScreenResolution) {
	assert(theScreenResolution);
	defaultGreyColorInner  = makecol(176, 176, 196);
	defaultGreyColorLighterBorder = makecol(252, 252, 252);
	defaultGreyColorDarkerBorder = makecol(84, 84, 120);
	screenResolution = theScreenResolution;
}

GuiShapeFactory::~GuiShapeFactory() {
}

GuiButton * GuiShapeFactory::createGrayButton(Rectangle * rect, std::string theLabel) {
	GuiButton * button = new GuiButton(rect, theLabel);
	button->setColors(defaultGreyColorDarkerBorder, defaultGreyColorLighterBorder, defaultGreyColorInner);
	return button;
}

GuiButton * GuiShapeFactory::createButtonWithBitmap(int x, int y, BITMAP * bmp, std::string theLabel) {
	assert(bmp);
	GuiButton * button = new GuiButton(x, y, bmp->w, bmp->h, theLabel);
	button->setBitmap(bmp);
	return button;
}

MainMenuDialog * GuiShapeFactory::createMainMenuDialog() {
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
	mainMenuDialog->setColors(defaultGreyColorDarkerBorder, defaultGreyColorLighterBorder, defaultGreyColorInner);

	//mainMenuDialog->setNewCampaignButton()
	return mainMenuDialog;
}
