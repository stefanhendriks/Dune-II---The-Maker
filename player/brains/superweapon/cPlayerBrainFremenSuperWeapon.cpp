#include <algorithm>
#include "include/d2tmh.h"
#include "cPlayerBrainFremenSuperWeapon.h"


namespace brains {

    cPlayerBrainFremenSuperWeapon::cPlayerBrainFremenSuperWeapon(cPlayer *player) : cPlayerBrain(player) {

    }

    cPlayerBrainFremenSuperWeapon::~cPlayerBrainFremenSuperWeapon() {

    }

    void cPlayerBrainFremenSuperWeapon::think() {
        // find any unit that does not attack, and let it attack an enemy?
        bool foundIdleUnit = false;
        std::vector<int> ids = player->getAllMyUnits();
        for (auto &id : ids) {
            cUnit &cUnit = unit[id];
            if (cUnit.isIdle()) {
                foundIdleUnit = true;
                break;
            }
        }

        if (!foundIdleUnit) {
//        char msg[255];
//        sprintf(msg, "cPlayerBrainFremenSuperWeapon::think() AI[%d] - NO idle unit(s) to attack with.", player->getId());
//        logbook(msg);
            return;
        }

        char msg[255];
        sprintf(msg, "cPlayerBrainFremenSuperWeapon::think() AI[%d] - found %d idle unit(s) to attack with.",
                player->getId(), ids.size());
        logbook(msg);

        // attack things!
        int cellToAttack = -1;

        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (i == player->getId()) continue; // skip self
            if (players[i].isSameTeamAs(player)) continue; // skip same team players

            // some chance to skip this player...
            if (rnd(100) > 70) {
                continue;
            }

            // nope! now choose which unit to attack
            std::vector<int> unitIds = players[i].getAllMyUnits();
            if (!unitIds.empty()) {
                std::random_shuffle(unitIds.begin(), unitIds.end());
                cellToAttack = unit[unitIds.front()].getCell();
                if (rnd(100) > 30) break;
            }

            // or which structure
            std::vector<int> structureIds = players[i].getAllMyStructuresAsId();
            if (!structureIds.empty()) {
                // pick structure to attack
                std::random_shuffle(structureIds.begin(), structureIds.end());
                cellToAttack = structure[structureIds.front()]->getCell();
                if (rnd(100) > 30) break;
            }
        }

        memset(msg, 0, sizeof(msg));
        sprintf(msg, "cPlayerBrainFremenSuperWeapon::think() - cellToAttack = %d", cellToAttack);
        player->log(msg);

        // order units to attack!
        for (auto &id : ids) {
            cUnit &pUnit = unit[id];
            if (!pUnit.isIdle()) continue;
            pUnit.attackAt(cellToAttack);
        }

        memset(msg, 0, sizeof(msg));
        sprintf(msg, "cPlayerBrainFremenSuperWeapon::think() - FINISHED");
        player->log(msg);
    }

    void cPlayerBrainFremenSuperWeapon::onNotify(const s_GameEvent &event) {
        // not interested
    }

    void cPlayerBrainFremenSuperWeapon::addBuildOrder(S_buildOrder order) {

    }

}