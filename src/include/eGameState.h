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
