#include "../include/d2tmh.h"

cParticleDrawer::cParticleDrawer() {

}

cParticleDrawer::~cParticleDrawer() {
}


void cParticleDrawer::drawLowerLayer() {
	drawLayer(1);
}

void cParticleDrawer::drawHigherLayer() {
	drawLayer(0);
}

void cParticleDrawer::drawLayer(int layer) {
	for (int i=0; i < MAX_PARTICLES; i++) {
        cParticle &pParticle = particle[i];
        if (pParticle.isValid() &&
            pParticle.getLayer() == layer) {
			pParticle.draw();
		}
	}
}
