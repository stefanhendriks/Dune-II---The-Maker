/**
 * @file cParticleDrawer.h
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
