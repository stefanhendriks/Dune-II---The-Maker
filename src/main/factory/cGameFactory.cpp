
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

		cSideBar * sidebar = cSideBarFactory::getInstance()->createSideBar(thePlayer, game.iMission);
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


// TODO: BECOMES OBSELETE, can be removed
void cGameFactory::createInteractionManagerForHumanPlayer(GameState state) {
	logbook("cGameFactory:createInteractionManagerForHumanPlayer [BEGIN]");
	logbook("cGameFactory:createInteractionManagerForHumanPlayer [END]");
}

void cGameFactory::createNewGameDrawerAndSetCreditsForHuman() {
	logbook("cGameFactory:createNewGameDrawerAndSetCreditsForHuman [BEGIN]");
	if (gameDrawer) {
		gameDrawer->destroy();
		delete gameDrawer;
		gameDrawer = NULL;
	}

	gameDrawer = new GameDrawer(&player[HUMAN]);
	gameDrawer->getCreditsDrawer()->setCreditsOfPlayer();
	logbook("cGameFactory:createNewGameDrawerAndSetCreditsForHuman [END]");
}

void cGameFactory::createMapClasses()
{
	delete mapUtils;
	mapUtils = NULL;
	delete map;
	map = NULL;
	delete mapCamera;
	mapCamera = NULL;

    mapCamera = new cMapCamera();
    map = new cMap(64, 64);
    mapUtils = new cMapUtils(map);
}

void cGameFactory::createMapClassAndNewDependenciesForGame(GameState state) {
	logbook("cGameFactory:createMapClassAndNewDependenciesForGame [BEGIN]");
    createMapClasses();
    createNewDependenciesForGame(state);
	logbook("cGameFactory:createMapClassAndNewDependenciesForGame [END]");
}

void cGameFactory::createNewDependenciesForGame(GameState state) {
	logbook("cGameFactory:createNewDependenciesForGame [BEGIN]");

	createDependenciesForPlayers();
	createGameControlsContextsForPlayers();

	createNewGameDrawerAndSetCreditsForHuman();
	createInteractionManagerForHumanPlayer(state);

	logbook("cGameFactory:createNewDependenciesForGame [END]");
}

void cGameFactory::destroyAll() {
	logbook("cGameFactory:destroyAll [BEGIN]");
	delete mapUtils;
	mapUtils = NULL;
	delete map;
	map = NULL;
	delete mapCamera;
	mapCamera = NULL;

	gameDrawer->destroy();

	delete gameDrawer;
	gameDrawer = NULL;

//	for (int i = HUMAN; i < MAX_PLAYERS; i++) {
//		cPlayer * thePlayer = &player[i];
//		delete thePlayer;
//		thePlayer = NULL;
//	}

	destroy_bitmap(bmp_fadeout);
	destroy_bitmap(bmp_screen);
	destroy_bitmap(bmp_throttle);
	destroy_bitmap(bmp_winlose);

	logbook("cGameFactory:destroyAll [END]");
}

cGameFactory * cGameFactory::getInstance() {
	if (instance == NULL) {
		instance = new cGameFactory();
	}
	return instance;
}
