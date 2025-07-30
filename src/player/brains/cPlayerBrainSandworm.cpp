#include "cPlayerBrainSandworm.h"

#include "d2tmc.h"
#include "player/cPlayer.h"

namespace brains {
cPlayerBrainSandworm::cPlayerBrainSandworm(cPlayer *player) : cPlayerBrain(player)
{
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
        cUnit &pSandWorm = unit[i];

        // when on guard
        bool allowedToMove = pSandWorm.TIMER_movewait < 1;
        if (pSandWorm.isIdle() && allowedToMove) {
            findRandomValidLocationToMoveToAndGoThere(pSandWorm);
        }
    }

}

void cPlayerBrainSandworm::findRandomValidLocationToMoveToAndGoThere(cUnit &pSandWorm) const
{
    // find new spot to go to
    for (int iTries = 0; iTries < 5; iTries++) {
        int iMoveTo = global_map.getRandomCellWithinMapWithSafeDistanceFromBorder(2);

        if (global_map.isCellPassableForWorm(iMoveTo)) {
            pSandWorm.move_to(iMoveTo);
            // found a spot, break out of this 'tries' loop
            break;
        }
    }
}

void cPlayerBrainSandworm::onNotifyGameEvent(const s_GameEvent &)
{

}

void cPlayerBrainSandworm::addBuildOrder(S_buildOrder)
{

}

void cPlayerBrainSandworm::thinkFast()
{

}
}
