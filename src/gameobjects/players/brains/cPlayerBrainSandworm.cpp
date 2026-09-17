/**
 * @file cPlayerBrainSandworm.cpp
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

#include "cPlayerBrainSandworm.h"
#include "context/cGameObjectContext.h"
#include "gameobjects/map/cMap.h"
#include "gameobjects/players/cPlayer.h"
#include "include/cAssert.h"

namespace brains {
cPlayerBrainSandworm::cPlayerBrainSandworm(cPlayer *player) : cPlayerBrain(player)
{
    d2tm_assert(player != nullptr);
    TIMER_think = 0;
}

cPlayerBrainSandworm::~cPlayerBrainSandworm()
{

}

void cPlayerBrainSandworm::think()
{
    TIMER_think++;
    if (TIMER_think < 5) {
        return;
    }

    TIMER_think = 0;
    // loop through all its worms and move them around
    const std::vector<int> &wormIds = player->getAllMyUnitsForType(SANDWORM);
    for (auto &i : wormIds) {
        cUnit *pSandWorm = m_objects->getUnit(i);

        // when on guard
        bool allowedToMove = pSandWorm->movewaitTimer.get() < 1;
        if (pSandWorm->isIdle() && allowedToMove) {
            findRandomValidLocationToMoveToAndGoThere(pSandWorm);
        }
    }

}

void cPlayerBrainSandworm::findRandomValidLocationToMoveToAndGoThere(cUnit *pSandWorm) const
{
    int placeToMoveTo = m_objects->getMap()->findRandomCellToMoveToForSandworm();

    if (placeToMoveTo > -1) {
        pSandWorm->move_to(placeToMoveTo);
    } else {
        pSandWorm->die(false, false);
    }
}

void cPlayerBrainSandworm::onNotifyGameEvent(const s_GameEvent &)
{

}

void cPlayerBrainSandworm::addBuildOrder(s_buildOrder)
{

}

void cPlayerBrainSandworm::thinkFast()
{

}
}
