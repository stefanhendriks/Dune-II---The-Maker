#include "d2tmh.h"
#include "cPlayerBrainMissionKind.h"


namespace brains {

    cPlayerBrainMissionKind::cPlayerBrainMissionKind(cPlayer *player, cPlayerBrainMission * mission) : player(player), mission(mission) {
        player->log("cPlayerBrainMissionKind() constructor");
    }

    cPlayerBrainMissionKind::~cPlayerBrainMissionKind() {

    }

    void cPlayerBrainMissionKind::log(const char *txt) {
        char msg[1024];
        sprintf(msg, "cPlayerBrainMissionKind | %s", txt);
        mission->log(txt);
    }

}