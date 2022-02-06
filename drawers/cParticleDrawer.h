#pragma once

#include <vector>

class cParticleDrawer {
	public:
		void drawLowerLayer();
		void drawTopLayer();
        void drawDebugInfo();

        void determineParticlesToDraw();

	private:
        std::vector<int> particlesLowerLayer;
        std::vector<int> particlesTopLayer;
};
