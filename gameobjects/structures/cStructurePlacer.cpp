/*
 * cStructurePlacer.cpp
 *
 *  Created on: 2-aug-2010
 *      Author: Stefan
 */

#include "../../include/d2tmh.h"

cStructurePlacer::cStructurePlacer(cPlayer * thePlayer) {
	assert(thePlayer);
    m_Player = thePlayer;
}

cStructurePlacer::~cStructurePlacer() {
    m_Player = NULL;
}

void cStructurePlacer::placeStructure(int destinationCell, int iStructureTypeId, int healthPercentage) {
	// create structure
	cStructureFactory::getInstance()->createStructure(destinationCell, iStructureTypeId, m_Player->getId(), healthPercentage);
}
