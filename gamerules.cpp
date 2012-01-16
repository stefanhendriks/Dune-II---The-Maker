#include "gamerules.h"

bool GameRules::shouldRemainAlive(int neighbours) {
	return neighbours == 2 || neighbours == 3;
}
