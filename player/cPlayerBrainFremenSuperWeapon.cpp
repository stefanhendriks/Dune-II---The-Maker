#include <algorithm>
#include "../include/d2tmh.h"

cPlayerBrainFremenSuperWeapon::cPlayerBrainFremenSuperWeapon(cPlayer * player) : cPlayerBrain(player) {

}

cPlayerBrainFremenSuperWeapon::~cPlayerBrainFremenSuperWeapon() {

}

void cPlayerBrainFremenSuperWeapon::think() {
    // find any unit that does not attack, and let it attack an enemy?
    bool foundIdleUnit = false;
    std::vector<int> ids = player_->getAllMyUnits();
    for (auto & id : ids) {
        cUnit &cUnit = unit[id];
        if (cUnit.isIdle()) {
            foundIdleUnit = true;
            break;
        }
    }

    if (!foundIdleUnit) {
        char msg[255];
        sprintf(msg, "think_fremen_superweapon AI[%d] - NO idle unit(s) to attack with. - done!", player_->getId());
        logbook(msg);
        return;
    }

    char msg[255];
    sprintf(msg, "think_fremen_superweapon AI[%d] - found idle unit(s) to attack with.", player_->getId());
    logbook(msg);

    // attack things!
    int unitIdToAttack = -1;
    int structureIdToAttack = -1;

    for (int i = 1; i < MAX_PLAYERS; i++) {
        if (i == player_->getId()) continue; // skip self
        if (player[i].isSameTeamAs(player_)) continue; // skip same team players

        if (rnd(100) > 70) {
            continue;
        }

        std::vector<int> unitIds = player[i].getAllMyUnits();
        if (!unitIds.empty()) {
            std::random_shuffle(unitIds.begin(), unitIds.end());
            unitIdToAttack = unitIds.front();
            if (rnd(100) > 30) break;
        }

        std::vector<int> structureIds = player[i].getAllMyStructures();
        if (!structureIds.empty()) {
            // pick structure to attack
            std::random_shuffle(structureIds.begin(), structureIds.end());
            structureIdToAttack = structureIds.front();
            if (rnd(100) > 30) break;
        }
    }

    memset(msg, 0, sizeof(msg));
    sprintf(msg, "think_fremen_superweapon AI[%d] - unitIdToAttack = %d, structureIdToAttack = %d.", player_->getId(), unitIdToAttack, structureIdToAttack);
    logbook(msg);

    // order units to attack!
    for (auto & id : ids) {
        cUnit &cUnit = unit[id];
        if (!cUnit.isIdle()) continue;
        if (structureIdToAttack > -1) {
            int cell = structure[structureIdToAttack]->getCell();
            UNIT_ORDER_ATTACK(id, cell, -1, structureIdToAttack, -1);
        } else if (unitIdToAttack > -1) {
            int cell = unit[unitIdToAttack].getCell();
            UNIT_ORDER_ATTACK(id, cell, unitIdToAttack, -1, -1);
        }
    }

    memset(msg, 0, sizeof(msg));
    sprintf(msg, "cPlayerBrainFremenSuperWeapon::think(), for player [%d] - FINISHED", player_->getId());
    logbook(msg);
}
