/*
 * cStructureDrawer.h
 *
 *  Created on: 23-okt-2010
 *      Author: Stefan
 *
 *  Responsible for drawing structures
 *
 */

#ifndef CSTRUCTUREDRAWER_H_
#define CSTRUCTUREDRAWER_H_

class cStructureDrawer {
	public:
		cStructureDrawer();
		~cStructureDrawer();

		void drawStructuresFirstLayer();
		void drawStructuresSecondLayer();
		void drawStructuresHealthBars();

		int getDrawXForStructure(int cell);
		int getDrawYForStructure(int cell);

	protected:
		void drawStructureHealthBar(int iStructure);
		void drawStructuresForLayer(int layer);
		void drawRectangeOfStructure(cAbstractStructure * theStructure, int color);

		void drawStructurePrebuildAnimation(cAbstractStructure * structure);
		void drawStructureAnimation(cAbstractStructure * structure);
		void drawStructureAnimationWindTrap(cAbstractStructure * structure);
		void drawStructureAnimationRefinery(cAbstractStructure * structure);
		void drawStructureAnimationTurret(cAbstractStructure * structure);
		void drawRepairFacility(cRepairFacility * repairFacility);

		int determinePreBuildAnimationIndex(cAbstractStructure * structure);
		void drawStructureForLayer(cAbstractStructure * structure, int layer);

	private:
		bool isUneven(int value);
};

#endif /* CSTRUCTUREDRAWER_H_ */
