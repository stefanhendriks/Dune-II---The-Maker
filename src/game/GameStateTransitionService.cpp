/**
 * @file GameStateTransitionService.cpp
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

#include "game/GameStateTransitionService.hpp"

#include "game/cGame.h"

GameStateTransitionService::GameStateTransitionService(cGame &game)
    : m_game(game)
{
}

void GameStateTransitionService::setNextState(int state) const
{
    m_game.setNextStateToTransitionTo(state);
}

void GameStateTransitionService::setNextStateWithFade(int state) const
{
    m_game.setNextStateToTransitionTo(state);
    m_game.initiateFadingOut();
}
