/**
 * @file eGameState.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

enum class eGameState : char {
    INITIALIZE = 0,        // initialize game
    OVER,                  // game over
    MENU,                  // in a menu
    PLAYING,               // playing the actual game
    BRIEFING,              // doing some briefing
    EDITOR,                // running the editor
    NEW_MAP_EDITOR,        // prepare new map editor parameters
    OPTIONS,               // options menu
    REGION,                // select a region / select your next conquest
    SELECT_HOUSE,          // pick a house
    INTRO,               
    TELLHOUSE,             // tell about the house
    WINNING,               // when winning, do some cool animation
    SCORING,               // crude mission scoring screen, shown after winning
    WINBRIEF,              // mentat chatter when won the mission
    LOSEBRIEF,             // briefing when losing
    LOSING,                // when losing, do something cool
    SKIRMISH,              // playing a skirmish mission!
    SETUPSKIRMISH,         // set up a skirmish game
    CREDITS,               // credits
    MISSIONSELECT,         // mission select
    VIDEO,                 // playing a video
    COUNT            
};

const char *gameStateToString(eGameState state);

// Game states (state machine)
inline constexpr int GAME_INITIALIZE = -1;      // initialize game
inline constexpr int GAME_OVER = 0;      // game over
inline constexpr int GAME_MENU = 1;      // in a menu
inline constexpr int GAME_PLAYING = 2;      // playing the actual game
inline constexpr int GAME_BRIEFING = 3;      // doing some briefing
inline constexpr int GAME_EDITOR = 4;      // running the editor
inline constexpr int GAME_OPTIONS = 5;		 // options menu
inline constexpr int GAME_REGION = 6;		 // select a region / select your next conquest
inline constexpr int GAME_SELECT_HOUSE = 8;		 // pick a house
inline constexpr int GAME_INTRO = 9;
inline constexpr int GAME_TELLHOUSE = 10;		 // tell about the house
inline constexpr int GAME_WINNING = 11;      // when winning, do some cool animation
inline constexpr int GAME_WINBRIEF = 12;      // mentat chatter when won the mission
inline constexpr int GAME_LOSEBRIEF = 13;      // briefing when losing
inline constexpr int GAME_LOSING = 14;      // when losing, do something cool
inline constexpr int GAME_SKIRMISH = 15;      // playing a skirmish mission!
inline constexpr int GAME_SETUPSKIRMISH = 16;    // set up a skirmish game
inline constexpr int GAME_CREDITS = 17;    // credits
inline constexpr int GAME_MISSIONSELECT = 18;    // mission select
inline constexpr int GAME_NEW_MAP_EDITOR = 19;    // new map editor
inline constexpr int GAME_SCORING = 20;      // crude mission scoring screen, shown after winning
inline constexpr int GAME_VIDEO = 21;           // playing a video
inline constexpr int GAME_MAX_STATES = 22;

[[deprecated]] // We should start using eGameState enum, get rid of int state
const char *stateToString(const int &state);