#include "include/d2tmh.h"

#include "cPlayerBrainMissionKindExplore.h"

namespace brains {

    cPlayerBrainMissionKindExplore::cPlayerBrainMissionKindExplore(cPlayer *player, cPlayerBrainMission * mission) :  cPlayerBrainMissionKind(player, mission) {
        targetCell = -1;
        logbook("cPlayerBrainMissionKindExplore() constructor");
    }

    cPlayerBrainMissionKindExplore::~cPlayerBrainMissionKindExplore() {

    }

    bool cPlayerBrainMissionKindExplore::think_SelectTarget() {
        targetCell = map.getRandomCellWithinMapWithSafeDistanceFromBorder(2);
        return true;
    }

    void cPlayerBrainMissionKindExplore::think_Execute() {
        const std::vector<int> &units = mission->getUnits();
        for (auto &myUnit : units) {
            cUnit &aUnit = unit[myUnit];
            if (aUnit.isValid()) {
                if (aUnit.isIdle()) {
                    if (map.distance(aUnit.getCell(), targetCell) < 4) {
                        targetCell = -1;
                        mission->changeState(PLAYERBRAINMISSION_STATE_SELECT_TARGET); // select new target
                    } else {
                        aUnit.move_to(targetCell);
                    }
                } else {
                    if (map.distance(aUnit.getCell(), targetCell) < 2) {
                        // almost there. Select new target.
                        targetCell = -1;
                        mission->changeState(PLAYERBRAINMISSION_STATE_SELECT_TARGET); // select new target
                    }
                }
            }
        }
    }

    void cPlayerBrainMissionKindExplore::onNotify(const s_GameEvent &event) {
        char msg[255];
        sprintf(msg, "cPlayerBrainMissionKindExplore::onNotify() -> %s", event.toString(event.eventType));
        player->log(msg);
    }

    cPlayerBrainMissionKind *cPlayerBrainMissionKindExplore::clone(cPlayer *player, cPlayerBrainMission * mission) {
        cPlayerBrainMissionKindExplore *copy = new cPlayerBrainMissionKindExplore(player, mission);
        copy->targetCell = targetCell;
        return copy;
    }

}