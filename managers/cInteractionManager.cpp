/*
 * cInteractionManager.cpp
 *
 *  Created on: 10-aug-2010
 *      Author: Stefan
 */

#include "../d2tmh.h"

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
}

void cInteractionManager::interactWithSidebar() {
	sidebar->thinkInteraction();
	miniMapDrawer->interact();
}
