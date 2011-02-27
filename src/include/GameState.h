#ifndef GAMESTATE_H_
#define GAMESTATE_H_

enum GameState {
	INITIAL=-1,	// initial state
	GAMEOVER=0,
	MAINMENU,	// in a menu
	PLAYING,	// playing the actual game
	BRIEFING,
	EDITING,
	OPTIONS,
	NEXTCONQUEST,
	SELECTHOUSE=8,	// pick a house
	INTRO,		// playing intro
	HOUSEINTRODUCTION,
	WINNING,	// when winning, do some cool animation
	WINBRIEF,	// mentat chatter when won the mission
	LOSEBRIEF,	// briefing when losing
	LOSING,		// when  losing, do something cool
	SKIRMISH,	// playing a skirmish mission!
	SETUPSKIRMISH,	// set up a skirmish game
};

#endif
