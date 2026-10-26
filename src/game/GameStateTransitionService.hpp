/**
 * @file GameStateTransitionService.hpp
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

#include <functional>
#include <utility>

class GameStateTransitionService {
public:
    using TransitionCallback = std::function<void(int)>;

    GameStateTransitionService(TransitionCallback setNextState, TransitionCallback setNextStateWithFade)
        : m_setNextState(std::move(setNextState)),
          m_setNextStateWithFade(std::move(setNextStateWithFade)) {
    }

    void setNextState(int state) const {
        m_setNextState(state);
    }

    void setNextStateWithFade(int state) const {
        m_setNextStateWithFade(state);
    }

private:
    TransitionCallback m_setNextState;
    TransitionCallback m_setNextStateWithFade;
};
