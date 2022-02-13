/*
 * cStructureDrawer.h
 *
 *  Created on: 23-okt-2010
 *      Author: Stefan
 *
 *  Responsible for drawing structures
 *
 */
#pragma once

#include "gameobjects/structures/cAbstractStructure.h"
#include "utils/cStructureUtils.h"

class cStructureDrawer {
	public:
		void drawStructuresFirstLayer();
		void drawStructuresSecondLayer();
		void drawStructuresHealthBars();

	protected:
		void drawStructureHealthBar(int iStructure);
		void drawStructuresForLayer(int layer);
		void drawRectangeOfStructure(cAbstractStructure * theStructure, int color);

		void drawStructurePrebuildAnimation(cAbstractStructure * structure);
		void drawStructureAnimation(cAbstractStructure * structure);
		void drawStructureAnimationWindTrap(cAbstractStructure * structure);
		void drawStructureAnimationRefinery(cAbstractStructure * structure);
		void drawStructureAnimationTurret(cAbstractStructure * structure);

		int determinePreBuildAnimationIndex(cAbstractStructure * structure);
		void drawStructureForLayer(cAbstractStructure * structure, int layer);

	private:
        cStructureUtils structureUtils;

    void renderIconOfUnitBeingRepaired(cAbstractStructure *structure) const;

    void renderIconThatStructureIsBeingRepaired(cAbstractStructure *structure) const;
};
