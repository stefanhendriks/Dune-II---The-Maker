#include "cPlayerBrainEmpty.h"
#include <cassert>

namespace brains {

cPlayerBrainEmpty::cPlayerBrainEmpty(cPlayer *player) : cPlayerBrain(player)
{
    assert(player != nullptr);
}

void cPlayerBrainEmpty::think()
{
    // NOOP
}

void cPlayerBrainEmpty::onNotifyGameEvent(const s_GameEvent &)
{
    // NOOP
}

void cPlayerBrainEmpty::addBuildOrder(S_buildOrder)
{
    // NOOP
}

void cPlayerBrainEmpty::thinkFast()
{
    // NOOP
}

}