#pragma once

#include "gameobjects/structures/cAbstractStructure.h"
#include "utils/cStructureUtils.h"

class cStructureDrawer {
public:
    void drawStructuresFirstLayer();
    void drawStructuresSecondLayer();
    void drawStructuresHealthBars();

protected:
    void drawStructureHealthBar(int iStructure);
    void drawStructuresForLayer(int layer);
    void drawRectangleOfStructure(cAbstractStructure *theStructure, SDL_Color color);
    void drawStructurePrebuildAnimation(cAbstractStructure *structure);
    void drawStructureAnimation(cAbstractStructure *structure);
    void drawStructureAnimationWindTrap(cAbstractStructure *structure);
    void drawStructureAnimationRefinery(cAbstractStructure *structure);
    void drawStructureAnimationTurret(cAbstractStructure *structure);
    void drawStructureForLayer(cAbstractStructure *structure, int layer);

    int determinePreBuildAnimationIndex(cAbstractStructure *structure);

private:
    cStructureUtils structureUtils;

    void renderIconOfUnitBeingRepaired(cAbstractStructure *structure) const;
    void renderIconThatStructureIsBeingRepaired(cAbstractStructure *structure) const;
};
