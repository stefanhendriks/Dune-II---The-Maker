/**
 * @file IStateCallbacks.hpp
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

#include "include/eGameState.h"

class cGameState;
enum class eMusicType;

/**
 * Every side effect of a state transition that is not pure state bookkeeping. Implemented by cGame, so the
 * GameStateMachine has no dependency on audio, drawing, players, time or the mouse.
 *
 * Data that is needed for a specific transition (mission, house, mission won) is read by the implementation.
 */
class IStateCallbacks {
public:
    virtual ~IStateCallbacks() = default;

    // called first for every transition to a different state, before anything changed
    virtual void onTransitionStarting(eGameState from, eGameState to) = 0;

    virtual void playMusic(eMusicType musicType) = 0;
    virtual void setCheatMode(bool enabled) = 0;
    virtual void setMouseNormal() = 0;
    virtual void takeBackgroundScreen() = 0;

    // state is the (new or reused) PLAYING state, already set as current state
    virtual void onTransitionToPlaying(cGameState *state, bool isNew) = 0;
    // resuming a mission from OPTIONS
    virtual void onTransitionToPlayingFromOptions() = 0;

    // called before the OPTIONS state is created
    virtual void onTransitionToOptions(bool fromPlaying) = 0;
    // OPTIONS was already there: remember where to go back to, or refresh when coming from MISSIONSELECT
    virtual void onTransitionToOptionsExisting(cGameState *state, eGameState from) = 0;

    // state is the freshly created REGION state (a cSelectYourNextConquestState)
    virtual void onTransitionToRegionNew(cGameState *state) = 0;
    // fromWinLoseBrief: the next mission must be set up
    virtual void onTransitionToRegionExisting(cGameState *state, bool fromWinLoseBrief) = 0;

    // called before the SETUPSKIRMISH state is created
    virtual void onTransitionToSkirmish() = 0;
    // called before the TELLHOUSE state is created
    virtual void onEnteringTellHouse() = 0;
    // called after the LOSEBRIEF state is created, plays the briefing music of the remembered campaign house
    virtual void onEnteringLoseBrief() = 0;
};
