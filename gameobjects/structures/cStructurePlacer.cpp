/*
 * cStructurePlacer.cpp
 *
 *  Created on: 2-aug-2010
 *      Author: Stefan
 */

#include "../../include/d2tmh.h"

cStructurePlacer::cStructurePlacer(cPlayer * thePlayer) {
	assert(thePlayer);
    player = thePlayer;
}

cStructurePlacer::~cStructurePlacer() {
    player = NULL;
}

// TODO: move to player
cAbstractStructure* cStructurePlacer::placeStructure(int destinationCell, int iStructureTypeId, int healthPercentage) {
	// create structure
	return cStructureFactory::getInstance()->createStructure(destinationCell, iStructureTypeId, player->getId(), healthPercentage);
}
