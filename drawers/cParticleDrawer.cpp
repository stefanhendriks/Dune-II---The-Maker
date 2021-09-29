#include "../include/d2tmh.h"
#include "cParticleDrawer.h"


cParticleDrawer::cParticleDrawer() {
    particlesTopLayer = std::vector<int>();
    particlesLowerLayer = std::vector<int>();
}

cParticleDrawer::~cParticleDrawer() {
    particlesLowerLayer.clear();
    particlesTopLayer.clear();
}

void cParticleDrawer::determineParticlesToDraw() {
    particlesLowerLayer.clear();
    particlesTopLayer.clear();
    for (int i=0; i < MAX_PARTICLES; i++) {
        cParticle &pParticle = particle[i];
        if (!pParticle.isValid()) continue;
        if (!pParticle.isWithinViewport(game.mapViewport)) continue;

        if (pParticle.getLayer() == D2TM_RENDER_LAYER_PARTICLE_BOTTOM) {
            particlesLowerLayer.push_back(i);
        } else if (pParticle.getLayer() == D2TM_RENDER_LAYER_PARTICLE_TOP) {
            particlesTopLayer.push_back(i);
        }
    }
}

void cParticleDrawer::drawLowerLayer() {
    for (auto &i : particlesLowerLayer) {
        particle[i].draw();
    }
}

void cParticleDrawer::drawTopLayer() {
    for (auto &i : particlesTopLayer) {
        particle[i].draw();
    }
}

void cParticleDrawer::drawDebugInfo() {
    cTextDrawer textDrawer(game_font);
    textDrawer.drawTextWithTwoIntegers(0, 100, "Visible particles BOTTOM: %d / TOP: %d", particlesLowerLayer.size(), particlesTopLayer.size());
}
