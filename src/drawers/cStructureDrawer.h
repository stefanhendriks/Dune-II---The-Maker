#pragma once

class GameContext;
class Graphics;
class SDLDrawer;
class cTextDrawer;
class cPlayer;

#include "gameobjects/structures/cAbstractStructure.h"
#include "utils/cStructureUtils.h"
#include "utils/Color.hpp"

class cStructureDrawer {
public:
    explicit cStructureDrawer(GameContext *ctx, cPlayer *player);
    ~cStructureDrawer() = default;
    void setPlayer(cPlayer *pPlayer) { m_player = pPlayer; }
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
    SDLDrawer* m_renderDrawer;
    cTextDrawer* m_textDrawer;
    Graphics *m_gfxinter;
    Graphics *m_gfxdata;
    cStructureUtils m_structureUtils;
    cPlayer *m_player;

    void renderIconOfUnitBeingRepaired(cAbstractStructure *structure) const;
    void renderIconThatStructureIsBeingRepaired(cAbstractStructure *structure) const;
};
