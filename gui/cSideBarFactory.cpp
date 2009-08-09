/*
 * cSideBarFactory.cpp
 *
 *  Created on: Aug 2, 2009
 *      Author: Stefan
 */

#include "..\d2tmh.h"

cSideBarFactory *cSideBarFactory::instance = NULL;

cSideBarFactory::cSideBarFactory() {
}

cSideBarFactory *cSideBarFactory::getInstance() {
	if (instance == NULL) {
		instance = new cSideBarFactory();
	}

	return instance;
}

// construct a cSideBar
cSideBar * cSideBarFactory::createSideBar(int techlevel, int house) {
	cLogger *logger = cLogger::getInstance();
	logger->logCommentLine("createSideBar");
	cSideBar *sidebar = new cSideBar();

	// lists are already constructed with default constructor. Now initialize
	// them according to the techLevel.
	for (int listId = LIST_NONE; listId < LIST_MAX; listId++) {
		logger->logCommentLine("createList");
		cBuildingList *list = cBuildingListFactory::getInstance()->createList(listId, techlevel, house);
		sidebar->setList(listId, list);
	}

	logger->logCommentLine("return Sidebar");
	return sidebar;
}

