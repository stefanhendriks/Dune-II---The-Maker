/*
 * cInteractionManager.cpp
 *
 *  Created on: 10-aug-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cInteractionManager::cInteractionManager(cPlayer * thePlayer) {
	assert(thePlayer);
	sidebar = thePlayer->getSideBar();
	miniMapDrawer = drawManager->getMiniMapDrawer();
}

cInteractionManager::~cInteractionManager() {
	sidebar = NULL;
	miniMapDrawer = NULL;
}

void cInteractionManager::interact() {
	interactWithSidebar();
	interactWithMapCamera();
}

void cInteractionManager::interactWithKeyboard(){
    keyboardManager.interact();
}

void cInteractionManager::interactWithSidebar() {
	sidebar->thinkInteraction();
	miniMapDrawer->interact();
}

void cInteractionManager::interactWithMapCamera() {
    // MOUSE WHEEL scrolling causes zooming in/out
    if (cMouse::isMouseScrolledUp()) {
        mapCamera->zoomOut();
    }

    if (cMouse::isMouseScrolledDown()) {
        mapCamera->zoomIn();
    }
}
