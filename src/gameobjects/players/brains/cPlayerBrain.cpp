#include "cPlayerBrain.h"
#include <cassert>

namespace brains {

cPlayerBrain::cPlayerBrain(cPlayer *player) : player(player)
{
    assert(player != nullptr);
}

}