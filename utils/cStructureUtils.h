/*
 * cStructureUtils.h
 *
 *  Created on: 2-aug-2010
 *      Author: Stefan
 */

#ifndef CSTRUCTUREUTILS_H_
#define CSTRUCTUREUTILS_H_

class cStructureUtils {
	public:
		cStructureUtils();
		~cStructureUtils();

		int findStructureToDeployUnit(cPlayer * player, int structureType);
		int findStarportToDeployUnit(cPlayer * player);
		int findStructureTypeByTypeOfList(cBuildingList *list, cBuildingListItem *item);
		int findClosestStructureTypeWhereNoUnitIsHeadingToComparedToCell(int cell, int structureType, cPlayer * player);

		int getHeightOfStructureTypeInCells(int structureType);
		int getWidthOfStructureTypeInCells(int structureType);

		void putStructureOnDimension(int dimensionId, cAbstractStructure * theStructure);

		bool isStructureVisibleOnScreen(cAbstractStructure *structure);
		bool isMouseOverStructure(cAbstractStructure *structure, int screenX, int screenY);

		int getTotalPowerOutForPlayer(cPlayer * player);
		int getTotalPowerUsageForPlayer(cPlayer * player);
		int getTotalSpiceCapacityForPlayer(cPlayer * player);
	protected:

	private:
		cCellCalculator * cellCalculator;

};

#endif /* CSTRUCTUREUTILS_H_ */
