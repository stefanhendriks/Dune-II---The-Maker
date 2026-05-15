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