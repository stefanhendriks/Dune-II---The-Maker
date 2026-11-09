/**
 * @file GameStateMachine.cpp
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

#include "game/GameStateMachine.hpp"

#include "game/IStateCallbacks.hpp"
#include "game/IStateCreator.hpp"
#include "include/definitions.h"
#include "utils/Log.h"

GameStateMachine::GameStateMachine(IStateCreator &creator, IStateCallbacks &callbacks) :
    m_creator(creator),
    m_callbacks(callbacks)
{
}

void GameStateMachine::dropOptionsState()
{
    cGameState *optionsState = nullptr;
    if (m_creator.hasState(eGameState::OPTIONS)) {
        optionsState = m_creator.getState(eGameState::OPTIONS);
    }

    if (m_currentState == optionsState) {
        m_currentState = nullptr;
    }
    m_creator.destroyState(eGameState::OPTIONS);
}

void GameStateMachine::transitionTo(eGameState newState)
{
    if (newState == m_state) {
        // ignore
        return;
    }

    m_callbacks.onTransitionStarting(m_state, newState);

    Logger::info(COMP_GAME, "GameStateMachine::transitionTo", "Setting state from {} to {}",
                 gameStateToString(m_state), gameStateToString(newState));

    if (newState != eGameState::INITIALIZE) {
        bool deleteOldState = (newState != eGameState::REGION &&
                               newState != eGameState::PLAYING &&
                               newState != eGameState::EDITOR &&
                               newState != eGameState::OPTIONS); // don't delete these states, but re-use!

        if (newState == eGameState::PLAYING || newState == eGameState::REGION) {
            // make sure to delete options menu now
            dropOptionsState();
        }

        if (newState == eGameState::SELECT_HOUSE) {
            // a new campaign starts, so drop the conquest state of the previous one (it would skip the intro)
            if (m_creator.hasState(eGameState::REGION) &&
                    m_currentState == m_creator.getState(eGameState::REGION)) {
                m_currentState = nullptr;
            }
            m_creator.destroyState(eGameState::REGION);
        }

        if (newState == eGameState::MISSIONSELECT) {
            deleteOldState = true; // delete old missionselect state everytime
        }

        if (newState == eGameState::PLAYING && m_state == eGameState::OPTIONS) {
            // Options was opened while a mission was in progress; resume it instead of
            // re-running the normal mission-start sequence (which would reset mission stats).
            m_callbacks.onTransitionToPlayingFromOptions();
            m_currentState = m_creator.getState(eGameState::PLAYING);
            m_state = newState;
            return;
        }

        cGameState *existingState = nullptr;
        // VIDEO is excluded: its state is created up front (createVideoState) and must be kept as is
        if (newState != eGameState::VIDEO) {
            if (deleteOldState) {
                m_creator.destroyState(newState);
            }
            if (m_creator.hasState(newState)) {
                existingState = m_creator.getState(newState);
            }
        }

        if (existingState) {
            // no need for re-creating state
            enterExistingState(newState, existingState);
        } else {
            enterNewState(newState);
        }
    }
    m_state = newState;
}

void GameStateMachine::enterExistingState(eGameState newState, cGameState *existingState)
{
    if (newState == eGameState::REGION) {
        m_callbacks.playMusic(eMusicType::CONQUEST);
        // came from a win/lose brief state, so make sure to set up the next state
        const bool fromWinLoseBrief = (m_state == eGameState::WINBRIEF || m_state == eGameState::LOSEBRIEF);
        m_callbacks.onTransitionToRegionExisting(existingState, fromWinLoseBrief);
    } else if (newState == eGameState::OPTIONS) {
        m_callbacks.takeBackgroundScreen();
        m_callbacks.onTransitionToOptionsExisting(existingState, m_state);
    } else if (newState == eGameState::PLAYING) {
        m_currentState = existingState;
        m_callbacks.onTransitionToPlaying(existingState, false);
    }

    m_currentState = existingState;
}

void GameStateMachine::enterNewState(eGameState newState)
{
    cGameState *newStatePtr = nullptr;

    switch (newState) {
        case eGameState::REGION:
            newStatePtr = m_creator.getState(eGameState::REGION);
            m_callbacks.onTransitionToRegionNew(newStatePtr);
            m_callbacks.playMusic(eMusicType::CONQUEST);
            break;
        case eGameState::SETUPSKIRMISH:
            m_callbacks.onTransitionToSkirmish();
            newStatePtr = m_creator.getState(eGameState::SETUPSKIRMISH);
            m_callbacks.playMusic(eMusicType::MENU);
            break;
        case eGameState::MENU:
            m_callbacks.setCheatMode(false);
            newStatePtr = m_creator.getState(eGameState::MENU);
            m_callbacks.playMusic(eMusicType::MENU);
            break;
        case eGameState::MISSIONSELECT:
            m_callbacks.setMouseNormal();
            newStatePtr = m_creator.getState(eGameState::MISSIONSELECT);
            break;
        case eGameState::OPTIONS:
            m_callbacks.takeBackgroundScreen();
            m_callbacks.setMouseNormal();
            m_callbacks.onTransitionToOptions(m_state == eGameState::PLAYING);
            newStatePtr = m_creator.getState(eGameState::OPTIONS);
            break;
        case eGameState::PLAYING:
            newStatePtr = m_creator.getState(eGameState::PLAYING);
            m_currentState = newStatePtr;
            m_callbacks.onTransitionToPlaying(newStatePtr, true);
            break;
        case eGameState::TELLHOUSE:
            m_callbacks.onEnteringTellHouse();
            newStatePtr = m_creator.getState(eGameState::TELLHOUSE);
            m_callbacks.playMusic(eMusicType::BRIEFING);
            break;
        case eGameState::BRIEFING:
            m_callbacks.setCheatMode(false);
            newStatePtr = m_creator.getState(eGameState::BRIEFING);
            m_callbacks.playMusic(eMusicType::BRIEFING);
            break;
        case eGameState::WINBRIEF:
            newStatePtr = m_creator.getState(eGameState::WINBRIEF);
            m_callbacks.playMusic(eMusicType::BRIEFING);
            break;
        case eGameState::LOSEBRIEF:
            newStatePtr = m_creator.getState(eGameState::LOSEBRIEF);
            m_callbacks.onEnteringLoseBrief();
            break;
        case eGameState::CREDITS:
        case eGameState::EDITOR:
        case eGameState::VIDEO:
        case eGameState::NEW_MAP_EDITOR:
        case eGameState::SELECT_HOUSE:
        case eGameState::LOSING:
        case eGameState::WINNING:
        case eGameState::SCORING:
            newStatePtr = m_creator.getState(newState);
            break;
        default:
            // no state object for the remaining values (INITIALIZE is handled by the caller)
            break;
    }
    m_currentState = newStatePtr;
}
