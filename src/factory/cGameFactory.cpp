
#include "../include/d2tmh.h"
//#include <assert.h>

//#include "../include/definitions.h"
//#include "../include/structs.h"
//
//#include "../sidebar/cBuildingListItem.h"
//#include "../sidebar/cBuildingList.h"
//#include "../upgrade/cBuildingListUpdater.h"
//#include "../player/cPlayer.h"
//
//#include "../controls/cMouse.h"
//#include "../controls/cGameControlsContext.h"
//
//#include "../managers/cInteractionManager.h"
//
//#include "../cGame.h"
#include "cGameFactory.h"

cGameFactory *cGameFactory::instance = NULL;

cGameFactory::cGameFactory() {
}

cGameFactory::~cGameFactory() {
}

void cGameFactory::createDependenciesForPlayers() {
	for (int i = HUMAN; i < MAX_PLAYERS; i++) {
		cPlayer * thePlayer = &player[i];
		thePlayer->setId(i);

		cItemBuilder * itemBuilder = new cItemBuilder(thePlayer);
		thePlayer->setItemBuilder(itemBuilder);

		cSideBar * sidebar = cSideBarFactory::getInstance()->createSideBar(&player[i], game.iMission, game.iHouse);
		thePlayer->setSideBar(sidebar);

		cBuildingListUpdater * buildingListUpdater = new cBuildingListUpdater(thePlayer);
		thePlayer->setBuildingListUpdater(buildingListUpdater);

		cStructurePlacer * structurePlacer = new cStructurePlacer(thePlayer);
		thePlayer->setStructurePlacer(structurePlacer);

		cUpgradeBuilder * upgradeBuilder = new cUpgradeBuilder(thePlayer);
		thePlayer->setUpgradeBuilder(upgradeBuilder);

		cOrderProcesser * orderProcesser = new cOrderProcesser(thePlayer);
		thePlayer->setOrderProcesser(orderProcesser);

		cGameControlsContext * gameControlsContext = new cGameControlsContext(thePlayer);
		thePlayer->setGameControlsContext(gameControlsContext);

		thePlayer->setTechLevel(game.iMission);
	}
}

void cGameFactory::createNewDependenciesForGame() {
	if (interactionManager) {
		delete interactionManager;
		interactionManager = NULL;
	}

	createDependenciesForPlayers();

	interactionManager = new cInteractionManager(&player[HUMAN]);

	if (mapUtils) {
		delete mapUtils;
		mapUtils = NULL;
	}

	if (map) {
		delete map;
		map = NULL;
	}

	if (mapCamera) {
		delete mapCamera;
		mapCamera = NULL;
	}

	mapCamera = new cMapCamera();
	map = new cMap(64, 64);
	mapUtils = new cMapUtils(map);
}

cGameFactory * cGameFactory::getInstance() {
	if (instance == NULL) {
		instance = new cGameFactory();
	}
	return instance;
}
