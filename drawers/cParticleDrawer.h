#ifndef CPARTICLEDRAWER_H_
#define CPARTICLEDRAWER_H_

class cParticleDrawer {
	public:
		cParticleDrawer();
		~cParticleDrawer();

		void drawLowerLayer();
		void drawHigherLayer();

	protected:
		void drawLayer(int layer);

	private:

};

#endif /* CPARTICLEDRAWER_H_ */
