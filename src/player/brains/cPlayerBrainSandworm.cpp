#include "cPlayerBrainSandworm.h"
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"

#include "game/cGame.h"
#include "include/d2tmc.h"
#include "map/cMap.h"
#include "player/cPlayer.h"
#include <cassert>

namespace brains {
cPlayerBrainSandworm::cPlayerBrainSandworm(cPlayer *player) : cPlayerBrain(player)
{
    assert(player != nullptr);
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
        cUnit *pSandWorm = game.m_gameObjectsContext->getUnit(i);

        // when on guard
        bool allowedToMove = pSandWorm->movewaitTimer.get() < 1;
        if (pSandWorm->isIdle() && allowedToMove) {
            findRandomValidLocationToMoveToAndGoThere(pSandWorm);
        }
    }

}

void cPlayerBrainSandworm::findRandomValidLocationToMoveToAndGoThere(cUnit *pSandWorm) const
{
    int placeToMoveTo = game.m_gameObjectsContext->getMap().findRandomCellToMoveToForSandworm();

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
