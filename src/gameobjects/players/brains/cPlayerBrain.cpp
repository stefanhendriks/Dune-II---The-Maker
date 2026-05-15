#include "cPlayerBrain.h"
#include "include/cAssert.h"

namespace brains {

cPlayerBrain::cPlayerBrain(cPlayer *player) : player(player)
{
    my_assert(player != nullptr);
}

}