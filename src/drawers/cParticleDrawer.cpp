/**
 * @file cParticleDrawer.cpp
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

#include "cParticleDrawer.h"

#include "include/sGameServices.h"
#include "drawers/cTextDrawer.h"
#include "gameobjects/particles/cParticle.h"
#include "gameobjects/particles/cParticles.h"
#include "gameobjects/structures/cStructures.h"
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"

void cParticleDrawer::serviceInit(sGameServices* s)
{
    m_objects = s->objects;
}

void cParticleDrawer::determineParticlesToDraw(const cRectangle &viewport)
{
    m_particlesLowerLayer.clear();
    m_particlesTopLayer.clear();
    for (auto &pParticle : m_objects->getParticles()) {
        if (!pParticle.isValid()) continue;
        if (!pParticle.isWithinViewport(viewport)) continue;

        if (pParticle.getLayer() == RenderLayerParticle::BOTTOM) {
            m_particlesLowerLayer.push_back(&pParticle);
        }
        else if (pParticle.getLayer() == RenderLayerParticle::TOP) {
            m_particlesTopLayer.push_back(&pParticle);
        }
    }
}

void cParticleDrawer::drawLowerLayer()
{
    for (auto *pParticle : m_particlesLowerLayer) {
        pParticle->draw();
    }
}

void cParticleDrawer::drawTopLayer()
{
    for (auto *pParticle : m_particlesTopLayer) {
        pParticle->draw();
    }
}

void cParticleDrawer::drawDebugInfo(cTextDrawer* textDrawer)
{
    textDrawer->drawText(0, 100,
                        std::format("Visible particles BOTTOM: {}/ TOP: {}", m_particlesLowerLayer.size(), m_particlesTopLayer.size()));
}
