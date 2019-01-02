/*
 * cCombatInteractionManager.cpp
 *
 *  Created on: 27 feb. 2011
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cCombatInteractionManager::cCombatInteractionManager(cPlayer * thePlayer) : cInteractionManager(thePlayer) {
	sidebar = player->getSideBar();
	assert(sidebar);
}

cCombatInteractionManager::~cCombatInteractionManager() {
}

void cCombatInteractionManager::interactWithMouse() {
	interactWithSidebar();
}

void cCombatInteractionManager::interactWithKeyboard() {
	cInteractionManager::interactWithKeyboard();
}

void cCombatInteractionManager::interactWithSidebar() {
	assert(sidebar);
	sidebar->thinkInteraction();
	cMiniMapDrawer * miniMapDrawer = gameDrawer->getMiniMapDrawer();
	assert(miniMapDrawer);
	miniMapDrawer->interact();
}
