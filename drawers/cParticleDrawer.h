#ifndef CPARTICLEDRAWER_H_
#define CPARTICLEDRAWER_H_

class cParticleDrawer {
	public:
		cParticleDrawer();
		~cParticleDrawer();

		void drawLowerLayer();
		void drawTopLayer();
        void drawDebugInfo();

        void determineParticlesToDraw();

	private:
        std::vector<int> particlesLowerLayer;
        std::vector<int> particlesTopLayer;
};

#endif /* CPARTICLEDRAWER_H_ */
