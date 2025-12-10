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

