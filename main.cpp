#include <iostream>

#include "gamerules.h"

using namespace std;

// for now playing around with the Game Of Life, along with some
// make file stuff (instead of letting an IDE generate it)
// and also with SublimeText 2 and GIT. All new stuff, oh joy!
// 
// Game Of Life: http://en.wikipedia.org/wiki/Conway's_Game_of_Life

void evaluateGameOfLifeRulesWithNeighbourCount(int neighbours) {
	GameRules gamerules;
	cout << "Evaluating rules with [" << neighbours << "] neighbours -- Start" << endl;
	cout << "With " << neighbours << " shouldRemainAlive returns " << gamerules.shouldRemainAlive(neighbours) << endl;
	cout << "With " << neighbours << " shouldRevive returns " << gamerules.shouldRevive(neighbours) << endl;
	cout << "With " << neighbours << " shouldDie returns " << gamerules.shouldDie(neighbours) << endl;
	cout << "DONE Evaluating rules with [" << neighbours << "] neighbours" << endl;
}

int main(int argc, char **argv) {
	cout << "Conway's Game Of Life" << endl;
	evaluateGameOfLifeRulesWithNeighbourCount(1);
	evaluateGameOfLifeRulesWithNeighbourCount(2);
	evaluateGameOfLifeRulesWithNeighbourCount(3);
	evaluateGameOfLifeRulesWithNeighbourCount(4);
	evaluateGameOfLifeRulesWithNeighbourCount(5);
	return 0;
}
