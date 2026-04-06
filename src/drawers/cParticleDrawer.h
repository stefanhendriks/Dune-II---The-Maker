#pragma once

#include <vector>
#include "utils/cRectangle.h"
#include "gameobjects/particles/cParticle.h"

class cTextDrawer;

class cParticleDrawer {
public:
    void drawLowerLayer();
    void drawTopLayer();
    void drawDebugInfo(cTextDrawer* textDrawer);

    void determineParticlesToDraw(const cRectangle &viewport);

private:
    std::vector<cParticle*> m_particlesLowerLayer;
    std::vector<cParticle*> m_particlesTopLayer;
};
