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

	protected:

	private:

};

#endif /* CSTRUCTUREUTILS_H_ */
