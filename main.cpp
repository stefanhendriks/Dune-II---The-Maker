#include <iostream>

#include "gamerules.h"

using namespace std;

// for now playing around with the Game Of Life, along with some
// make file stuff (instead of letting an IDE generate it)
// and also with SublimeText 2 and GIT. All new stuff, oh joy!
// 
// Game Of Life: http://en.wikipedia.org/wiki/Conway's_Game_of_Life

void printOutputRemainAliveForNeighbourCount(int neighbours) {
	GameRules gamerules;
	bool result = gamerules.shouldRemainAlive(neighbours);
	cout << "With " << neighbours << " shouldRemainAlive returns " << result << endl;
}

int main(int argc, char **argv) {
	cout << "Conway's Game Of Life" << endl;
	printOutputRemainAliveForNeighbourCount(2);
	printOutputRemainAliveForNeighbourCount(3);
	printOutputRemainAliveForNeighbourCount(4);
	return 0;
}
