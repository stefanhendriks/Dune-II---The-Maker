/*
 * cPlaceItDrawer.h
 *
 *  Created on: 12-aug-2010
 *      Author: Stefan
 */

#ifndef CPLACEITDRAWER_H_
#define CPLACEITDRAWER_H_

class cPlaceItDrawer {
	public:
		cPlaceItDrawer();
		~cPlaceItDrawer();

		void draw(cBuildingListItem *itemToPlace);

	protected:
		void drawStructureIdAtCell(cBuildingListItem *itemToPlace, int cell);
		void drawStatusOfStructureAtCell(cBuildingListItem *itemToPlace, int cell);

	private:

		cCellCalculator * cellCalculator;
};

#endif /* CPLACEITDRAWER_H_ */
