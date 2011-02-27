
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
	logbook("cGameFactory:createDependenciesForPlayers [BEGIN]");

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

		thePlayer->setTechLevel(game.iMission);
	}
	logbook("cGameFactory:createDependenciesForPlayers [END]");

}

void cGameFactory::createGameControlsContextsForPlayers() {
	logbook("cGameFactory:createGameControlsContextsForPlayers [BEGIN]");

	for (int i = HUMAN; i < MAX_PLAYERS; i++) {
		cPlayer * thePlayer = &player[i];
		thePlayer->setId(i);

		cGameControlsContext * gameControlsContext = new cGameControlsContext(thePlayer);
		thePlayer->setGameControlsContext(gameControlsContext);
	}

	logbook("cGameFactory:createGameControlsContextsForPlayers [END]");
}

void cGameFactory::createInteractionManagerForHumanPlayer(GameState state) {
	logbook("cGameFactory:createInteractionManagerForHumanPlayer [BEGIN]");
	if (interactionManager) {
		delete interactionManager;
		interactionManager = NULL;
	}
	cPlayer * thePlayer = &player[HUMAN];
	switch (state) {
		case MAINMENU:
			interactionManager = new cMenuInteractionManager(thePlayer);
			break;
		case PLAYING:
			interactionManager = new cCombatInteractionManager(thePlayer);
			break;
		case BRIEFING:
			interactionManager = new cMenuInteractionManager(thePlayer);
			break;
		default:
			interactionManager = NULL;
	}
	assert(interactionManager);
	logbook("cGameFactory:createInteractionManagerForHumanPlayer [END]");
}

void cGameFactory::createNewDependenciesForGame(GameState state) {
	logbook("cGameFactory:createNewDependenciesForGame [BEGIN]");
	createInteractionManagerForHumanPlayer(state);
    createDependenciesForPlayers();
	createGameControlsContextsForPlayers();

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
	logbook("cGameFactory:createNewDependenciesForGame [END]");
}

cGameFactory * cGameFactory::getInstance() {
	if (instance == NULL) {
		instance = new cGameFactory();
	}
	return instance;
}
