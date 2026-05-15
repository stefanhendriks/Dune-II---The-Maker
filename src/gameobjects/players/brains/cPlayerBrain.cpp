#include "cPlayerBrain.h"
#include "include/cAssert.h"

namespace brains {

cPlayerBrain::cPlayerBrain(cPlayer *player) : player(player)
{
    d2tm_assert(player != nullptr);
}

}