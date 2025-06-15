#pragma once

#include "cTextDrawer.h"
#include "observers/cInputObserver.h"
#include "sidebar/cBuildingList.h"

class cPlayer;

class cBuildingListDrawer : cInputObserver {

public:
	explicit cBuildingListDrawer(cPlayer *player);

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    void drawList(cBuildingList *list, int listIDToDraw);
    void drawButtonHoverRectangle(cBuildingList *list);
	void drawButton(cBuildingList *list, bool pressed);

	cBuildingListItem *isOverItemCoordinates(cBuildingList *list, int x, int y);

    int getDrawX();
    int getDrawY();

    void setPlayer(cPlayer * player);

protected:

private:
	// the list to draw
	cTextDrawer m_textDrawer;

	cPlayer* m_player;

    bool m_renderListIds;

    void drawStructureSize(int structureId, int x, int y);
    void drawList(cBuildingList *list, bool shouldDrawStructureSize);
    void drawListWithStructures(cBuildingList *list);		// used for const yard
    void drawListWithUnitsOrAbilities(cBuildingList *list); // used for units or palace / starport

    bool isOverItemCoordinates_Boolean(int x, int y, int drawX, int drawY);

    void onKeyHold(const cKeyboardEvent &event);

    void onKeyPressed(const cKeyboardEvent &event);
};
