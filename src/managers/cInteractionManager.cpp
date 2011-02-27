/*
 * cInteractionManager.cpp
 *
 *  Created on: 10-aug-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cInteractionManager::cInteractionManager(cPlayer * thePlayer) {
	assert(thePlayer);
	player = thePlayer;
}

cInteractionManager::~cInteractionManager() {
}

void cInteractionManager::interactWithMouse() {
}

void cInteractionManager::interactWithKeyboard() {
	keyboardManager.interact();
}
