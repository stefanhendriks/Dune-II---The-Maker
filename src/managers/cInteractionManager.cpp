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
	miniMapDrawer = gameDrawer->getMiniMapDrawer();
}

cInteractionManager::~cInteractionManager() {
	sidebar = NULL;
	miniMapDrawer = NULL;
}

void cInteractionManager::interact() {
	interactWithSidebar();
}

void cInteractionManager::interactWithKeyboard(){
    keyboardManager.interact();
}

void cInteractionManager::interactWithSidebar() {
	sidebar->thinkInteraction();
	miniMapDrawer->interact();
}
