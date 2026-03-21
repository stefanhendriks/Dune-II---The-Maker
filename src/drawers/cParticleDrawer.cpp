#include "cParticleDrawer.h"

#include "d2tmc.h"
#include "drawers/cTextDrawer.h"
#include "gameobjects/particles/cParticle.h"

void cParticleDrawer::determineParticlesToDraw(const cRectangle &viewport)
{
    particlesLowerLayer.clear();
    particlesTopLayer.clear();
    for (int i=0; i < game.getParticles().size(); i++) {
        cParticle &pParticle = game.getParticles()[i];
        if (!pParticle.isValid()) continue;
        if (!pParticle.isWithinViewport(viewport)) continue;

        if (pParticle.getLayer() == RenderLayerParticle::BOTTOM) {
            particlesLowerLayer.push_back(i);
        }
        else if (pParticle.getLayer() == RenderLayerParticle::TOP) {
            particlesTopLayer.push_back(i);
        }
    }
}

void cParticleDrawer::drawLowerLayer()
{
    for (auto &i : particlesLowerLayer) {
        game.getParticles()[i].draw();
    }
}

void cParticleDrawer::drawTopLayer()
{
    for (auto &i : particlesTopLayer) {
        game.getParticles()[i].draw();
    }
}

void cParticleDrawer::drawDebugInfo(cTextDrawer* textDrawer)
{
    textDrawer->drawText(0, 100,
                        std::format("Visible particles BOTTOM: {}/ TOP: {}", particlesLowerLayer.size(), particlesTopLayer.size()));
}
