#pragma once

#include "sidebar/cBuildingListItem.h"

class GameContext;
class cPlayer;
class Graphics;
class SDLDrawer;
class cStructureUtils;
class cGameObjectContext;
class cInfoContext;
struct sGameServices;

class cPlaceItDrawer {
public:
    explicit cPlaceItDrawer(GameContext *ctx, cPlayer *thePlayer, cStructureUtils *structureUtils);

    ~cPlaceItDrawer();

    void serviceInit(sGameServices* services);

    void draw(cBuildingListItem *itemToPlace, int mouseCell);

protected:
    void drawStructureIdAtMousePos(cBuildingListItem *itemToPlace);

    void drawStatusOfStructureAtCell(cBuildingListItem *itemToPlace, int mouseCell);

private:
    cStructureUtils* m_structureUtils = nullptr;
    cPlayer *m_player;
    GameContext *m_ctx;
    SDLDrawer *m_renderDrawer;
    Graphics *m_gfxdata;
    cGameObjectContext* m_objects = nullptr;
    cInfoContext* m_infos = nullptr;
};
