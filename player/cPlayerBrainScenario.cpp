#include <algorithm>
#include "../include/d2tmh.h"


cPlayerBrainScenario::cPlayerBrainScenario(cPlayer * player) : cPlayerBrain(player) {
    TIMER_initialDelay = 100;
    TIMER_processBuildOrders = 100;
    shouldScanBaseAndStoreIt = true;
    myBase = std::vector<S_structurePosition>();
    buildOrders = std::vector<S_buildOrder>();
}

cPlayerBrainScenario::~cPlayerBrainScenario() {

}

void cPlayerBrainScenario::think() {
    TIMER_initialDelay--;
    if (TIMER_initialDelay > 0) {
        char msg[255];
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "cPlayerBrainScenario::think(), for player [%d] - Initial Delay still active...", player->getId());
        logbook(msg);
        return;
    }

    if (shouldScanBaseAndStoreIt) {
        storeMyBase();
        shouldScanBaseAndStoreIt = false;
        TIMER_scanBase = 100;
    }

    if (TIMER_scanBase > 0) {
        TIMER_scanBase--;
    } else {
        // once scanned, keep an eye on any destroyed structures
        scanBase();
        TIMER_scanBase = 25;
    }

    if (TIMER_processBuildOrders > 0) {
        TIMER_processBuildOrders--;
    } else {
        TIMER_processBuildOrders = 25;
        processBuildOrders();

        if (player->isBuildingStructureAwaitingPlacement()) {
            int structureType = player->getStructureTypeBeingBuilt();
            for (auto & buildOrder : buildOrders) {
                if (buildOrder.buildType != eBuildType::STRUCTURE) {
                    continue;
                }

                if (buildOrder.buildId == structureType) {
                    int iCll = buildOrder.placeAt;

                    cBuildingListItem *pItem = player->getStructureBuildingListItemBeingBuilt();
                    player->placeStructure(iCll, pItem);

                    buildOrder.state = buildOrder::eBuildOrderState::REMOVEME;
                    break;
                }
            }
        }
    }

    // now do some real stuff

    char msg[255];
    memset(msg, 0, sizeof(msg));
    sprintf(msg, "cPlayerBrainScenario::think(), for player [%d] - FINISHED", player->getId());
    logbook(msg);
}

/**
 * This function scans the world for structures belonging to this player, remembers the structure types and places
 * in case they got destroyed so they can be rebuild.
 */
void cPlayerBrainScenario::storeMyBase() {
    const std::vector<int> &ids = player->getAllMyStructuresAsId();
    for (auto & id : ids) {
        cAbstractStructure *pStructure = structure[id];
        // we can assume the pointers point to something valid, because getAllMyStructuresAsId does some checking
        S_structurePosition position = {
                .cell = pStructure->getCell(),
                .type = pStructure->getType(),
                .structureId = pStructure->getStructureId(),
                .isDestroyed = pStructure->isDead()
        };
        myBase.push_back(position);
    }

    char msg[255];
    sprintf(msg, "cPlayerBrainScenario::storeMyBase() - player [%d / %s] scanned base, and concluded this is the base:", player->getId(), player->getHouseName().c_str());
    logbook(msg);

    int id = 0;
    for (auto & myStructure : myBase) {
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "[%d] - type = STRUCTURE, buildId = %d (=%s), at cell %d", id, myStructure.type, structures[myStructure.type].name, myStructure.cell);
        logbook(msg);

        id++;
    }

}

void cPlayerBrainScenario::addBuildOrder(S_buildOrder order) {
    // check if we can find a similar build order
    for (auto & buildOrder : buildOrders) {
        if (buildOrder.buildType != order.buildType) continue;
        if (buildOrder.buildId != order.buildId) continue;

        if (order.buildType == eBuildType::STRUCTURE) {
            if (buildOrder.placeAt != order.placeAt) continue;
        }

        // found same, if so, then simply increase priority?
        buildOrder.priority++;
        return; // stop
    }
    buildOrders.push_back(order);

    // re-order based on priority
    std::sort( buildOrders.begin(), buildOrders.end(), [ ]( const S_buildOrder& lhs, const S_buildOrder& rhs )
    {
        return lhs.priority > rhs.priority;
    });

    char msg[255];
    sprintf(msg, "cPlayerBrainScenario::addBuildOrder() - player [%d / %s] results into the following build orders:", player->getId(), player->getHouseName().c_str());
    logbook(msg);

    int id = 0;
    for (auto & buildOrder : buildOrders) {
        memset(msg, 0, sizeof(msg));
        if (buildOrder.buildType == eBuildType::UNIT) {
            sprintf(msg, "[%d] - type = UNIT, buildId = %d (=%s), priority = %d", id, buildOrder.buildId, units[buildOrder.buildId].name, buildOrder.priority);
        } else if (buildOrder.buildType == eBuildType::STRUCTURE) {
            sprintf(msg, "[%d] - type = STRUCTURE, buildId = %d (=%s), priority = %d, place at %d", id, buildOrder.buildId, structures[buildOrder.buildId].name, buildOrder.priority, buildOrder.placeAt);
        } else if (buildOrder.buildType == eBuildType::SPECIAL) {
            sprintf(msg, "[%d] - type = SPECIAL, buildId = %d (=%s), priority = %d", id, buildOrder.buildId, specials[buildOrder.buildId].description, buildOrder.priority);
        } else if (buildOrder.buildType == eBuildType::BULLET) {
            sprintf(msg, "[%d] - type = SPECIAL, buildId = %d (=NOT YET IMPLEMENTED), priority = %d", id, buildOrder.buildId, buildOrder.priority);
        }
        logbook(msg);

        id++;
    }
}

void cPlayerBrainScenario::processBuildOrders() {
    // check if we can find a similar build order
    for (auto & buildOrder : buildOrders) {
        if (buildOrder.state != buildOrder::eBuildOrderState::PROCESSME) continue; // only process those which are marked

        if (buildOrder.buildType == eBuildType::STRUCTURE) {
            if (player->canBuildStructure(buildOrder.buildId) == eCantBuildReason::NONE) {
                if (player->startBuildingStructure(buildOrder.buildId)) {
                    buildOrder.state = buildOrder::eBuildOrderState::BUILDING;
                }
            }
        } else if (buildOrder.buildType == eBuildType::UNIT) {
            if (player->canBuildUnit(buildOrder.buildId) == eCantBuildReason::NONE) {
                if (player->startBuildingUnit(buildOrder.buildId)) {
                    buildOrder.state = buildOrder::eBuildOrderState::REMOVEME;
                }
            }
        }
    }

    // delete any buildOrders which are flagged to remove
    buildOrders.erase(
    std::remove_if(
        buildOrders.begin(),
        buildOrders.end(),
            [](const S_buildOrder &o) { return o.state == buildOrder::eBuildOrderState::REMOVEME; }),
    buildOrders.end()
    );
}

// TODO: make this a listener/observer so we don't need to do a check like this, but we can
// receive an event that a structure got destroyed and hence we are interested in it so we can rebuild it...
void cPlayerBrainScenario::scanBase() {
    // check if my base is still in order...
    for (auto & structurePosition : myBase) {
        if (structurePosition.isDestroyed) {
            // check at position if another structure has been placed
            int structureIdOnMap = map.getCellIdStructuresLayer(structurePosition.cell);
            if (structureIdOnMap < 0) {
                continue; // skip
            }
            cAbstractStructure *pStructure = structure[structureIdOnMap];
            if (!pStructure) continue;
            if (!pStructure->isValid()) continue;
            if (pStructure->isDead()) continue;
            if (pStructure->getType() != structurePosition.type) continue; // not same type
            if (!pStructure->belongsTo(player)) continue; // not my structure :(

            // seems my structure, same type, same location.
            structurePosition.structureId = structureIdOnMap;
            structurePosition.isDestroyed = false; // no longer destroyed!
            continue; // skip these, we already have build orders for these.
        }

        cAbstractStructure *pStructure = structure[structurePosition.structureId];
        bool isAllOk = true;
        if (pStructure != nullptr) {
            if (pStructure->isDead() || !pStructure->belongsTo(player)) {
                isAllOk = false;
            }
        } else {
            isAllOk = false; // structure no longer exists (invalid pointer)
        }

        if (!isAllOk) {
            // ok this is not good, mark it as destroyed
            structurePosition.isDestroyed = true;
            // and order to
            addBuildOrder((S_buildOrder) {
                    buildType : eBuildType::STRUCTURE,
                    priority : 1,
                    buildId : structurePosition.type,
                    placeAt : structurePosition.cell,
                    state : buildOrder::eBuildOrderState::PROCESSME,
            });
        }
    }

    // do repairs?
}
