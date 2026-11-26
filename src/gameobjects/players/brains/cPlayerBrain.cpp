/**
 * @file cPlayerBrain.cpp
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

#include "cPlayerBrain.h"
#include "context/cGameObjectContext.h"
#include "context/cInfoContext.h"
#include "gameobjects/players/cPlayer.h"
#include "game/cGameSettings.h"
#include "context/GameContext.hpp"
#include "include/sGameServices.h"
#include "include/cAssert.h"

namespace brains {

cPlayerBrain::~cPlayerBrain() = default;

cPlayerBrain::cPlayerBrain(cPlayer *player) : player(player)
{
    d2tm_assert(player != nullptr);
    // brains created in cGame::init() come before the player has its infos; they are replaced before a game starts
    if (player->getInfos() != nullptr) {
        m_aiParameters = player->getInfos()->getAiParameters();
    }
}

void cPlayerBrain::serviceInit(sGameServices* services)
{
    m_objects = services->objects;
    m_info = services->info;
    m_settings = services->settings;
    m_eventEmitter = services->eventEmitter;
}

}