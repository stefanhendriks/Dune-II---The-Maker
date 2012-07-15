/*
 * cBuildingListDrawer.h
 *
 *  Created on: Aug 1, 2009
 *      Author: Stefan
 */

#ifndef CBUILDINGLISTDRAWER_H_
#define CBUILDINGLISTDRAWER_H_

class BuildingListDrawer {

	public:
		BuildingListDrawer();

		void drawList(cBuildingList *list, int listIDToDraw, int startId);
		void drawButton(cBuildingList *list, bool pressed);

		bool isOverItem(cBuildingList *list, int x, int y);
		cBuildingListItem *isOverItemCoordinates(cBuildingList *list, int x, int y);

	protected:
		int getDrawX();
		int getDrawY();

	private:
		void drawStructureSize(int structureId, int x, int y);
		void drawList(cBuildingList *list, int listIDToDraw, int startId, bool shouldDrawStructureSize);
		void drawListWithStructures(cBuildingList *list, int listIDToDraw, int startId); // used for const yard
		void drawListWithUnitsOrAbilities(cBuildingList *list, int listIDToDraw, int startId); // used for units or palace / starport

		bool isOverItemCoordinates_Boolean(int x, int y, int drawX, int drawY);
		static const int ICON_HEIGHT = 48;

		// the list to draw
		cBuildingList *list;
		int maximumItemsToDraw;
		int maxListYCoordinate;

};

#endif /* CBUILDINGLISTDRAWER_H_ */
