#include "d2tmh.h"

namespace brains {

    cPlayerBrainMissionKind::cPlayerBrainMissionKind(cPlayer *player, cPlayerBrainMission * mission) : player(player), mission(mission) {
        logbook("cPlayerBrainMissionKind() constructor");
    }

    cPlayerBrainMissionKind::~cPlayerBrainMissionKind() {

    }


}