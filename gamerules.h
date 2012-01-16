#ifndef GAMERULES_H
#define GAMERULES_H

class GameRules {
	
	public:
		bool shouldRemainAlive(int neighbours);
		bool shouldRevive(int neighbours);
		bool shouldDie(int neighbours);

};


#endif // GAMERULES_H
