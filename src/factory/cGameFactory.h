#ifndef CGAMEFACTORY_H_
#define CGAMEFACTORY_H_

class cGameFactory {
	public:
		cGameFactory();

		static cGameFactory *getInstance();

		void createNewDependenciesForGame(GameState state);
		void createGameControlsContextsForPlayers();
	    void createInteractionManagerForHumanPlayer(GameState state);
	    void createNewGameDrawerAndSetCreditsForHuman();

	protected:
		~cGameFactory();
		void createDependenciesForPlayers();

	private:
		static cGameFactory *instance;
};

#endif /* CGAMEFACTORY_H_ */
