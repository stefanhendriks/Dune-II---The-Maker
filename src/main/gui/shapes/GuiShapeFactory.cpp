#include "../../include/d2tmh.h"

GuiShapeFactory::GuiShapeFactory(ScreenResolution * theScreenResolution) {
	assert(theScreenResolution);
	screenResolution = theScreenResolution;
}

GuiShapeFactory::~GuiShapeFactory() {
}

GuiButton * GuiShapeFactory::createGrayButton(Rectangle * rect, std::string theLabel) {
	GuiButton * button = new GuiButton(rect, theLabel);
	button->setColors(guiColors.getMenuDarkBorderColor(), guiColors.getMenuLightBorderColor(), guiColors.getMenuGreyColor());
	return button;
}

GuiButton * GuiShapeFactory::createButtonWithBitmap(int x, int y, BITMAP * bmp, std::string theLabel) {
	assert(bmp);
	GuiButton * button = new GuiButton(x, y, bmp->w, bmp->h, theLabel);
	button->setBitmap(bmp);
	return button;
}

MainMenuDialog * GuiShapeFactory::createMainMenuDialog() {

}
