/*
 * cGuiShapeFactory.cpp
 *
 *  Created on: 26-aug-2011
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

#include <string>

cGuiShapeFactory::cGuiShapeFactory() {
}

cGuiShapeFactory::~cGuiShapeFactory() {
}

cGuiButton * cGuiShapeFactory::createGrayButton(cRectangle * rect, std::string theLabel) {
	cGuiButton * button = new cGuiButton(rect, theLabel);
	int inner = makecol(176, 176, 196);
	int lightColor = makecol(252, 252, 252);
	int darkColor = makecol(84, 84, 120);
	button->setColors(darkColor, lightColor, inner);
	return button;
}

cGuiButton * cGuiShapeFactory::createButtonWithBitmap(int x, int y, BITMAP * bmp, std::string theLabel) {
	assert(bmp);
	cGuiButton * button = new cGuiButton(x, y, bmp->w, bmp->h, theLabel);
	button->setBitmap(bmp);
	return button;
}

cMainMenuDialog * cGuiShapeFactory::createMainMenuDialog(cRectangle * rect) {
	assert(rect);
	cMainMenuDialog * mainMenuDialog = new cMainMenuDialog(rect);
	//mainMenuDialog->setNewCampaignButton()
	return mainMenuDialog;
}
