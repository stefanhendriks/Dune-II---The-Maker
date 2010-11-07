/*
 * cParticleDrawer.cpp
 *
 *  Created on: 11-aug-2010
 *      Author: Stefan
 */

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
		if (particle[i].isValid() && particle[i].layer == layer) {
			particle[i].draw();
		}
	}
}
