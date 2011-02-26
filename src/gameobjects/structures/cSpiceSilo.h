// Structure class

class cSpiceSilo : public cAbstractStructure {
	private:

	public:
		cSpiceSilo();
		~cSpiceSilo();

		// overloaded functions
		void think();
		void think_animation();
		void think_guard();

		void draw(int iStage);

		int getType();

		int getSpiceSiloCapacity();
};

