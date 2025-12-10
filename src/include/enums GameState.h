#pragma once

enum class GameState : char {
    INITIALIZE = 0,        // initialize game
    OVER,                  // game over
    MENU,                  // in a menu
    PLAYING,               // playing the actual game
    BRIEFING,              // doing some briefing
    EDITING,               // running the editor
    OPTIONS,               // options menu
    REGION,                // select a region / select your next conquest
    SELECT_HOUSE,          // pick a house
    INTRO,               
    TELLHOUSE,             // tell about the house
    WINNING,               // when winning, do some cool animation
    WINBRIEF,              // mentat chatter when won the mission
    LOSEBRIEF,             // briefing when losing
    LOSING,                // when losing, do something cool
    SKIRMISH,              // playing a skirmish mission!
    SETUPSKIRMISH,         // set up a skirmish game
    CREDITS,               // credits
    MISSIONSELECT,         // mission select
    COUNT            
};

// Game states (state machine)
#define GAME_INITIALIZE  -1      // initialize game
#define GAME_OVER         0      // game over
#define GAME_MENU         1      // in a menu
#define GAME_PLAYING      2      // playing the actual game
#define GAME_BRIEFING     3      // doing some briefing
#define GAME_EDITING      4      // running the editor
#define GAME_OPTIONS	  5		 // options menu
#define GAME_REGION		  6		 // select a region / select your next conquest
#define GAME_SELECT_HOUSE 8		 // pick a house
#define GAME_INTRO	      9
#define GAME_TELLHOUSE	 10		 // tell about the house
#define GAME_WINNING     11      // when winning, do some cool animation
#define GAME_WINBRIEF    12      // mentat chatter when won the mission
#define GAME_LOSEBRIEF   13      // briefing when losing
#define GAME_LOSING      14      // when losing, do something cool
#define GAME_SKIRMISH    15      // playing a skirmish mission!
#define GAME_SETUPSKIRMISH 16    // set up a skirmish game
#define GAME_CREDITS     17    // credits
#define GAME_MISSIONSELECT  18    // mission select
#define GAME_MAX_STATES 19

const char *stateToString(const int &state);