#include "cPlayerBrainFremenSuperWeapon.h"

#include "d2tmc.h"
#include "player/cPlayer.h"

#include <fmt/core.h>

#include <algorithm>
#include <random>

namespace brains {

cPlayerBrainFremenSuperWeapon::cPlayerBrainFremenSuperWeapon(cPlayer *player) : cPlayerBrain(player)
{

}

void cPlayerBrainFremenSuperWeapon::think()
{
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
        return;
    }

    logbook(fmt::format("cPlayerBrainFremenSuperWeapon::think() AI[{}] - found {} idle unit(s) to attack with.",
                        player->getId(), ids.size()));

    // attack things!
    int cellToAttack = -1;
    std::random_device rd;
    std::mt19937 g(rd());

    for (int i = 0; i < MAX_PLAYERS; i++) {
        cPlayer *pPlayer = &players[i];
        if (pPlayer == nullptr) continue;
        if (pPlayer == player) continue; // skip self
        if (pPlayer->isSameTeamAs(player)) continue; // skip same team players

        // some chance to skip this player...
        if (rnd(100) > 70) {
            continue;
        }

        // nope! now choose which unit to attack
        std::vector<int> unitIds = pPlayer->getAllMyUnits();
        if (!unitIds.empty()) {
            std::shuffle(unitIds.begin(), unitIds.end(), g);
            cellToAttack = unit[unitIds.front()].getCell();
            if (rnd(100) > 30) break;
        }

        // or which structure
        std::vector<int> structureIds = pPlayer->getAllMyStructuresAsId();
        if (!structureIds.empty()) {
            // pick structure to attack
            std::shuffle(structureIds.begin(), structureIds.end(), g);
            cellToAttack = structure[structureIds.front()]->getCell();
            if (rnd(100) > 30) break;
        }
    }

    player->log(fmt::format("cPlayerBrainFremenSuperWeapon::think() - cellToAttack = {}", cellToAttack));

    // order units to attack!
    for (auto &id : ids) {
        cUnit &pUnit = unit[id];
        if (!pUnit.isIdle()) continue;
        pUnit.attackAt(cellToAttack);
    }

    player->log("cPlayerBrainFremenSuperWeapon::think() - FINISHED");
}

void cPlayerBrainFremenSuperWeapon::onNotifyGameEvent(const s_GameEvent &)
{
    // not interested
}

void cPlayerBrainFremenSuperWeapon::addBuildOrder(S_buildOrder)
{

}

void cPlayerBrainFremenSuperWeapon::thinkFast()
{

}

}