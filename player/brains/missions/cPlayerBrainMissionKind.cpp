#include "d2tmh.h"

namespace brains {

    cPlayerBrainMissionKind::cPlayerBrainMissionKind(cPlayer *player, cPlayerBrainMission * mission) : player(player), mission(mission) {
        player->log("cPlayerBrainMissionKind() constructor");
    }

    cPlayerBrainMissionKind::~cPlayerBrainMissionKind() {

    }

}