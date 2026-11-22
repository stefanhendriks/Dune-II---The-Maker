/**
 * @file cMouseState.cpp
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

#include "cMouseState.h"
#include "controls/eMouseTile.h"
#include "include/cAssert.h"

cMouseState::cMouseState(cPlayer *player, cGameControlsContext *context, cMouse *mouse) :
    m_player(player),
    m_context(context),
    m_mouse(mouse),
    m_mouseTile(eMouseTile::NORMAL)
{
    d2tm_assert(player!=nullptr);
    d2tm_assert(context!=nullptr);
    d2tm_assert(mouse!=nullptr); 
}