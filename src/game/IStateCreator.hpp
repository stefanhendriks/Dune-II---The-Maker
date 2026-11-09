/**
 * @file IStateCreator.hpp
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

/**
 * What the GameStateMachine needs from whoever creates and caches the game states (cCreatorState).
 * A separate interface so the state machine can be unit tested without every concrete game state.
 */
class IStateCreator {
public:
    virtual ~IStateCreator() = default;

    // creates the state when it does not exist yet
    virtual cGameState *getState(eGameState gameState) = 0;
    [[nodiscard]] virtual bool hasState(eGameState gameState) const = 0;
    virtual void destroyState(eGameState gameState) = 0;
};
