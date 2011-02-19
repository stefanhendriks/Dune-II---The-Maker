#ifndef GAMESTATE_H_
#define GAMESTATE_H_

enum GameState {
	INITIAL=-1,	// initial state
	OVER=0,		// game over
	INMENU,		// in a menu
	PLAYING,	// playing the actual game
	BRIEFING,	// doing some briefing
	EDITING,	// running the editor
	OPTIONS,	// options menu
	REGION,		// select a region
	HOUSE=8,	// pick a house
	INTRO,		// playing intro
	TELLHOUSE,	// tell about the house
	WINNING,	// when winning, do some cool animation
	WINBRIEF,	// mentat chatter when won the mission
	LOSEBRIEF,	// briefing when losing
	LOSING,		// when  losing, do something cool
	SKIRMISH,	// playing a skirmish mission!
	SETUPSKIRMISH,	// set up a skirmish game
};

#endif
