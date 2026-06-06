#pragma once

#include <vector>
#include "utils/cRectangle.h"
#include "gameobjects/particles/cParticle.h"

class cTextDrawer;
class cGameObjectContext;
struct sGameServices;

class cParticleDrawer {
public:
    void serviceInit(sGameServices* services);
    void drawLowerLayer();
    void drawTopLayer();
    void drawDebugInfo(cTextDrawer* textDrawer);

    void determineParticlesToDraw(const cRectangle &viewport);

private:
    std::vector<cParticle*> m_particlesLowerLayer;
    std::vector<cParticle*> m_particlesTopLayer;
    cGameObjectContext* m_objects = nullptr;
};
