#pragma once

#include <vector>
#include "utils/cRectangle.h"

class cParticleDrawer {
public:
    void drawLowerLayer();
    void drawTopLayer();
    void drawDebugInfo();

    void determineParticlesToDraw(const cRectangle &viewport);

private:
    std::vector<int> particlesLowerLayer;
    std::vector<int> particlesTopLayer;
};
