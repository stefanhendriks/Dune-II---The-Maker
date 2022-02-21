#include "cRespondToThreatAction.h"

#include "player/cPlayer.h"
#include "d2tmc.h"

#include <vector>

cRespondToThreatAction::cRespondToThreatAction(cPlayer *player, cUnit * threat, cUnit * victim, int cellOriginOfThreat, int maxUnitsToOrder)
:
m_player(player),
m_threat(threat),
m_cellOriginOfThreat(cellOriginOfThreat),
m_victim(victim),
m_maxUnitsToOrder(maxUnitsToOrder)
{

}

void cRespondToThreatAction::execute() {
    // be aware: this also returns the victim unit!
    const std::vector<sEntityForDistance> &units = m_player->getAllMyUnitsOrderClosestToCell(m_cellOriginOfThreat);

    int skipThisUnit = -1;
    if (m_victim) {
        if (m_threat && !m_threat->isInfantryUnit()) {
            if (m_victim->isHarvester()) {
                if (m_victim->isIdle()) {
                    eHeadTowardsStructureResult result = m_victim->findBestStructureCandidateAndHeadTowardsItOrWait(
                            REFINERY,
                            true,
                            INTENT_UNLOAD_SPICE);

                    if (result == eHeadTowardsStructureResult::FAILED_NO_STRUCTURE_AVAILABLE) {
                        m_victim->retreatToNearbyBase();
                    }
                }
            }

            // do not engage this unit
            skipThisUnit = m_victim->iID;
        }
    }

    if (m_threat) {
        if (m_threat->isAirbornUnit()) {
            int unitsOrdered = 0;
            // find units that can counter-attack an air unit
            for (auto &ufd: units) {
                cUnit &pUnit = unit[ufd.entityId];
                if (pUnit.iID == skipThisUnit) continue;
                if (!pUnit.isIdle()) continue;
                if (!pUnit.canAttackAirUnits()) continue;
                if (pUnit.isHarvester()) continue; // harvesters cannot attack air units
                if (pUnit.isAirbornUnit()) continue; // you cannot order air units

                // move unit to where air unit is/was, so we get close to counter-attack
                pUnit.move_to(m_cellOriginOfThreat);
                unitsOrdered++;

                if (unitsOrdered > m_maxUnitsToOrder) break;
            }
            return;
        }
    }

    int unitsOrdered = 0;

    for (auto & ufd : units) {
        cUnit &pUnit = unit[ufd.entityId];
        if (pUnit.iID == skipThisUnit) continue;
        if (!pUnit.isIdle()) continue;
        if (!pUnit.isAttackingUnit()) continue; // is a unit that is used generally for attacking
        if (pUnit.isAirbornUnit()) continue; // you cannot order air units

        // TODO:
        // we can do more smart things here depending on the kind of unit that attacks us
        // and thus which unit we should send to counter-attack?
        pUnit.attackAt(m_cellOriginOfThreat);
        unitsOrdered++;

        if (unitsOrdered > m_maxUnitsToOrder) break;
    }
}