#pragma once

class cBuildingListDrawer {

public:
	cBuildingListDrawer(cPlayer *thePlayer);
	~cBuildingListDrawer();

	void drawList(cBuildingList *list, int listIDToDraw);
    void drawButtonHoverRectangle(cBuildingList *list);
	void drawButton(cBuildingList *list, bool pressed);

	cBuildingListItem *isOverItemCoordinates(cBuildingList *list, int x, int y);

    int getDrawX();
    int getDrawY();

    void setPlayer(cPlayer * thePlayer);

protected:

private:
	void drawStructureSize(int structureId, int x, int y);
	void drawList(cBuildingList *list, bool shouldDrawStructureSize);
	void drawListWithStructures(cBuildingList *list);		// used for const yard
	void drawListWithUnitsOrAbilities(cBuildingList *list); // used for units or palace / starport

	bool isOverItemCoordinates_Boolean(int x, int y, int drawX, int drawY);

	// the list to draw
	cTextDrawer *textDrawer;

	cPlayer * player;
};
