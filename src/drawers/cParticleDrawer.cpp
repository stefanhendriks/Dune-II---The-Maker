#include "cParticleDrawer.h"

#include "game/cGame.h"
#include "include/d2tmc.h"
#include "drawers/cTextDrawer.h"
#include "gameobjects/particles/cParticle.h"
#include "gameobjects/particles/cParticles.h"
#include "gameobjects/structures/cStructures.h"
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"

void cParticleDrawer::determineParticlesToDraw(const cRectangle &viewport)
{
    m_particlesLowerLayer.clear();
    m_particlesTopLayer.clear();
    for (auto &pParticle : game.m_gameObjectsContext->getParticles()) {
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
