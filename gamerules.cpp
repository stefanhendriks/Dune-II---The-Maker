#include "gamerules.h"

bool GameRules::shouldRemainAlive(int neighbours) {
	return neighbours == 2 || neighbours == 3;
}

bool GameRules::shouldRevive(int neighbours) {
	return neighbours == 3;	
}

bool GameRules::shouldDie(int neighbours) {
	return neighbours < 2 || neighbours > 3;
}

