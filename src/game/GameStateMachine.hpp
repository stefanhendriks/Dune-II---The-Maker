/**
 * @file GameStateMachine.hpp
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
class IStateCallbacks;
class IStateCreator;

/**
 * Knows which game state is active and what has to happen when switching to another one: which cached
 * states are kept, re-used or thrown away. Side effects go through IStateCallbacks.
 */
class GameStateMachine {
public:
    GameStateMachine(IStateCreator &creator, IStateCallbacks &callbacks);

    void transitionTo(eGameState newState);

    [[nodiscard]] eGameState getState() const {
        return m_state;
    }

    [[nodiscard]] cGameState *getCurrentState() const {
        return m_currentState;
    }

    // the state objects are about to be destroyed
    void clearCurrentState() {
        m_currentState = nullptr;
    }

    // back to INITIALIZE without any transition
    void resetState() {
        m_state = eGameState::INITIALIZE;
    }

private:
    IStateCreator &m_creator;
    IStateCallbacks &m_callbacks;
    eGameState m_state = eGameState::INITIALIZE;
    cGameState *m_currentState = nullptr;

    void dropOptionsState();
    void enterNewState(eGameState newState);
    void enterExistingState(eGameState newState, cGameState *existingState);
};
