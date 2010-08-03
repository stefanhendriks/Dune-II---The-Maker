/*
 * cStructurePlacer.cpp
 *
 *  Created on: 2-aug-2010
 *      Author: Stefan
 */

#include "..\d2tmh.h"

cStructurePlacer::cStructurePlacer(cPlayer * thePlayer) {
	assert(thePlayer);
	player = thePlayer;
}

cStructurePlacer::~cStructurePlacer() {
	player = NULL;
}

void cStructurePlacer::placeStructure(int destinationCell, int iStructureTypeId, int healthPercentage) {

	// create structure
	cStructureFactory::getInstance()->createStructure(destinationCell, iStructureTypeId, player->getId(), healthPercentage);

	// handle update
	cBuildingListUpdater * buildingListUpdater = player->getBuildingListUpdater();
	buildingListUpdater->updateStructureCreated(iStructureTypeId);
}
