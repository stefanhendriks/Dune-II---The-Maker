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
#include "game/cGameInterface.h"
#include "game/cGameSettings.h"
#include "context/GameContext.hpp"
#include "include/sGameServices.h"
#include "include/cAssert.h"

namespace brains {

cPlayerBrain::cPlayerBrain(cPlayer *player) : player(player)
{
    d2tm_assert(player != nullptr);
}

void cPlayerBrain::serviceInit(sGameServices* services)
{
    m_objects = services->objects;
    m_info = services->info;
    m_settings = services->settings;
    m_interface = services->ctx->getGameInterface();
}

}