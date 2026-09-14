/**
 * @file cMouseState.cpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#include "cMouseState.h"
// until we have our own data structure to abstract Allegro away, we have tight coupling here
#include "data/gfxdata.h"

#include "include/cAssert.h"

cMouseState::cMouseState(cPlayer *player, cGameControlsContext *context, cMouse *mouse) :
    m_player(player),
    m_context(context),
    m_mouse(mouse),
    m_mouseTile(MOUSE_NORMAL)
{
    d2tm_assert(player!=nullptr);
    d2tm_assert(context!=nullptr);
    d2tm_assert(mouse!=nullptr); 
}