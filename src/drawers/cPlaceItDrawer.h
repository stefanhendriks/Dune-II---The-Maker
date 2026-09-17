/**
 * @file cPlaceItDrawer.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

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
    SDLDrawer *m_sdlDrawer;
    Graphics *m_gfxdata;
    cGameObjectContext* m_objects = nullptr;
    cInfoContext* m_infos = nullptr;
};
