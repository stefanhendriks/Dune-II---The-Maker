#pragma once

#include <vector>
#include "utils/cRectangle.h"

class cTextDrawer;

class cParticleDrawer {
public:
    void drawLowerLayer();
    void drawTopLayer();
    void drawDebugInfo(cTextDrawer* textDrawer);

    void determineParticlesToDraw(const cRectangle &viewport);

private:
    std::vector<int> particlesLowerLayer;
    std::vector<int> particlesTopLayer;
};
