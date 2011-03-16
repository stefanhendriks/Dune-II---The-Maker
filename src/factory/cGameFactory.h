#ifndef CGAMEFACTORY_H_
#define CGAMEFACTORY_H_

class cGameFactory {
	public:
		cGameFactory();

		static cGameFactory *getInstance();

		void createMapClassAndNewDependenciesForGame(GameState state);
		void createNewDependenciesForGame(GameState state);

		void createGameControlsContextsForPlayers();
		void createInteractionManagerForHumanPlayer(GameState state);
		void createNewGameDrawerAndSetCreditsForHuman();

		void destroyAll();

	protected:
		~cGameFactory();

	private:
		static cGameFactory *instance;

		void createMapClasses();
		void createDependenciesForPlayers();

};

#endif /* CGAMEFACTORY_H_ */
