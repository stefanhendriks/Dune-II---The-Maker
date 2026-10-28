/**
 * @file cGameInterface.cpp
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

#include "game/cGameInterface.h"
#include "game/cGame.h"
#include "include/sGameEvent.h"

#include <format>

cGameInterface::cGameInterface(cGame* game)
    : m_game(game)
{
}

// void cGameInterface::loadSkirmishMaps() const
// {
//     m_game->loadSkirmishMaps();
// }

void cGameInterface::initSkirmish() const
{
    m_game->initSkirmish();
}

// void cGameInterface::loadMapFromEditor(s_PreviewMap *map) const
// {
//     m_game->loadMapFromEditor(map);
// }

void cGameInterface::setPlayerToInteractFor(cPlayer *pPlayer) const
{
    m_game->setPlayerToInteractFor(pPlayer);
}

void cGameInterface::setupPlayers() const
{
    m_game->setupPlayers();
}

void cGameInterface::drawTextFps() const
{
    m_game->drawTextFps();
}

void cGameInterface::drawTextTime() const
{
    m_game->drawTextTime();
}

void cGameInterface::loadMapFromEditor(int iSkirmishMap) const
{
    m_game->loadMapFromEditor(iSkirmishMap);
}

void cGameInterface::loadScenario() const
{
    m_game->loadScenario();
}
