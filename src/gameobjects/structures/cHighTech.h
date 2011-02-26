// Structure class

class cHighTech : public cAbstractStructure {
	private:
		void think_animation_unitDeploy();

	public:
		cHighTech();
		~cHighTech();

		// overloaded functions
		void think();
		void think_animation();
		void think_guard();

		void draw(int iStage);

		int getType();

};

