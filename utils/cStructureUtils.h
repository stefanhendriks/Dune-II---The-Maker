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
		int findClosestStructureTypeToCell(int cell, int structureType, cPlayer * player);

		int getStructureWidthInPixels(int structureType);
		int getStructureHeightInPixels(int structureType);

		int getStructureHeightInPixels(cAbstractStructure * theStructure);
		int getStructureWidthInPixels(cAbstractStructure * theStructure);

		int getHeightOfStructureTypeInCells(int structureType);
		int getWidthOfStructureTypeInCells(int structureType);

		void putStructureOnDimension(int dimensionId, cAbstractStructure * theStructure);

	protected:

	private:
		cCellCalculator * cellCalculator;

};

#endif /* CSTRUCTUREUTILS_H_ */
