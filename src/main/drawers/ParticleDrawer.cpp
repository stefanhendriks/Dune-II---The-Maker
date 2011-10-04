#include "../include/d2tmh.h"

ParticleDrawer::ParticleDrawer() {

}

ParticleDrawer::~ParticleDrawer() {
}

void ParticleDrawer::drawLayerBeneathUnits() {
	drawLayer(1);
}

void ParticleDrawer::drawHigherLayer() {
	drawLayer(0);
}

void ParticleDrawer::drawLayer(int layer) {
	for (int i = 0; i < MAX_PARTICLES; i++) {
		if (particle[i].isValid() && particle[i].layer == layer) {
			particle[i].draw();
		}
	}
}
