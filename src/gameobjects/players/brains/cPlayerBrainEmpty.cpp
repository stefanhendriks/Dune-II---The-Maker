/**
 * @file cPlayerBrainEmpty.cpp
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

#include "cPlayerBrainEmpty.h"
#include "include/cAssert.h"

namespace brains {

cPlayerBrainEmpty::cPlayerBrainEmpty(cPlayer *player) : cPlayerBrain(player)
{
    d2tm_assert(player != nullptr);
}

void cPlayerBrainEmpty::think()
{
    // NOOP
}

void cPlayerBrainEmpty::onNotifyGameEvent(const s_GameEvent &)
{
    // NOOP
}

void cPlayerBrainEmpty::addBuildOrder(s_buildOrder)
{
    // NOOP
}

void cPlayerBrainEmpty::thinkFast()
{
    // NOOP
}

}