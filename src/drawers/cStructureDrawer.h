/**
 * @file cStructureDrawer.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

class GameContext;
class Graphics;
class SDLDrawer;
class cTextDrawer;
class cPlayer;
class cGameObjectContext;
class cInfoContext;
class cMapCamera;
struct sGameServices;

#include "gameobjects/structures/cAbstractStructure.h"
#include "utils/cStructureUtils.h"
#include "utils/Color.hpp"

class cStructureDrawer {
public:
    explicit cStructureDrawer(GameContext *ctx, cPlayer *player, cStructureUtils *structureUtils);
    ~cStructureDrawer() = default;
    void setPlayer(cPlayer *pPlayer) { m_player = pPlayer; }
    void serviceInit(sGameServices* services);
    void drawStructuresFirstLayer();
    void drawStructuresSecondLayer();
    void drawStructuresHealthBars();

protected:
    void drawStructureHealthBar(int iStructure);
    void drawStructuresForLayer(int layer);
    void drawRectangleOfStructure(cAbstractStructure *theStructure, Color color);
    void drawStructurePrebuildAnimation(cAbstractStructure *structure);
    void drawStructureAnimation(cAbstractStructure *structure);
    void drawStructureAnimationRefinery(cAbstractStructure *structure);
    void drawStructureAnimationTurret(cAbstractStructure *structure);
    void drawStructureForLayer(cAbstractStructure *structure, int layer);

    int determinePreBuildAnimationIndex(cAbstractStructure *structure);

private:
    SDLDrawer* m_sdlDrawer;
    cTextDrawer* m_textDrawer;
    Graphics *m_gfxinter;
    Graphics *m_gfxdata;
    cStructureUtils* m_structureUtils = nullptr;
    cPlayer *m_player;
    GameContext* m_ctx = nullptr;
    cGameObjectContext* m_objects = nullptr;
    cInfoContext* m_infos = nullptr;
    cMapCamera* m_mapCamera = nullptr;

    void renderIconOfUnitBeingRepaired(cAbstractStructure *structure) const;
    void renderIconThatStructureIsBeingRepaired(cAbstractStructure *structure) const;
};
