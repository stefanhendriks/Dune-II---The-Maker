#include "cPlayerBrain.h"
#include "context/cGameObjectContext.h"
#include "context/cInfoContext.h"
#include "game/cGameInterface.h"
#include "game/cGameSettings.h"
#include "context/GameContext.hpp"
#include "include/sGameServices.h"
#include "include/cAssert.h"

namespace brains {

cPlayerBrain::cPlayerBrain(cPlayer *player) : player(player)
{
    d2tm_assert(player != nullptr);
}

void cPlayerBrain::serviceInit(sGameServices* services)
{
    m_objects = services->objects;
    m_info = services->info;
    m_settings = services->settings;
    m_interface = services->ctx->getGameInterface();
}

}