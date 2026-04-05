#pragma once

#include "sidebar/cBuildingListItem.h"
#include "utils/cStructureUtils.h"

class GameContext;
class cPlayer;
class Graphics;
class SDLDrawer;

class cPlaceItDrawer {
public:
    explicit cPlaceItDrawer(GameContext *ctx, cPlayer *thePlayer);

    ~cPlaceItDrawer();

    void draw(cBuildingListItem *itemToPlace, int mouseCell);

protected:
    void drawStructureIdAtMousePos(cBuildingListItem *itemToPlace);

    void drawStatusOfStructureAtCell(cBuildingListItem *itemToPlace, int mouseCell);

private:
    cStructureUtils m_structureUtils;
    cPlayer *m_player;
    GameContext *m_ctx;
    SDLDrawer *m_renderDrawer;
    Graphics *m_gfxdata;
};
