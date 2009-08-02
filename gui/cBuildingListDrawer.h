/*
 * cBuildingListDrawer.h
 *
 *  Created on: Aug 1, 2009
 *      Author: Stefan
 */

#ifndef CBUILDINGLISTDRAWER_H_
#define CBUILDINGLISTDRAWER_H_

class cBuildingListDrawer {

public:
	cBuildingListDrawer();

	void drawList(cBuildingList *list, int listIDToDraw, int startId);
	void drawButton(cBuildingList *list, bool pressed);

	bool isOverItem(cBuildingList *list, int x, int y);

private:
	void drawStructureSize(int structureId, int x, int y);
	void drawList(cBuildingList *list, int listIDToDraw, int startId, bool shouldDrawStructureSize);
	void drawListWithStructures(cBuildingList *list, int listIDToDraw, int startId);		// used for const yard
	void drawListWithUnitsOrAbilities(cBuildingList *list, int listIDToDraw, int startId); // used for units or palace / starport

	bool isOverItemCoordinates(int x, int y, int drawX, int drawY);

	// the list to draw
	cBuildingList *list;

};

#endif /* CBUILDINGLISTDRAWER_H_ */
