#include "d2tmh.h"
#include "cPlayerBrainMissionKind.h"


namespace brains {

    cPlayerBrainMissionKind::cPlayerBrainMissionKind(cPlayer *player, cPlayerBrainMission * mission) : player(player), mission(mission) {
        player->log("cPlayerBrainMissionKind() constructor");
    }

    cPlayerBrainMissionKind::~cPlayerBrainMissionKind() {

    }

    /**
     * Logs for a mission, do not call from the MissionKind constructors, as this will result into a SIGSEV (since the
     * mission->log() uses the missionKind as well, it will result into a SIGSEV when being cloned etc)
     * @param txt
     */
    void cPlayerBrainMissionKind::log(const char *txt) {
        char msg[1024];
        sprintf(msg, "cPlayerBrainMissionKind | %s", txt);
        mission->log(txt);
    }

}