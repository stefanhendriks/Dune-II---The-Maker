/**
 * @file cParticleDrawer.h
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
