#pragma once

#include "sidebar/cBuildingListItem.h"
#include "utils/cStructureUtils.h"

class cPlayer;

class cPlaceItDrawer {
public:
    explicit cPlaceItDrawer(cPlayer *thePlayer);

    ~cPlaceItDrawer();

    void draw(cBuildingListItem *itemToPlace, int mouseCell);

protected:
    void drawStructureIdAtMousePos(cBuildingListItem *itemToPlace);

    void drawStatusOfStructureAtCell(cBuildingListItem *itemToPlace, int mouseCell);

private:
    cStructureUtils structureUtils;

    cPlayer *player;
};
