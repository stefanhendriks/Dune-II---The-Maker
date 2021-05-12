#include <algorithm>
#include "../include/d2tmh.h"
#include "cPlayerBrainScenario.h"


cPlayerBrainScenario::cPlayerBrainScenario(cPlayer * player) : cPlayerBrain(player) {
    TIMER_initialDelay = 100;
    TIMER_processBuildOrders = 100;
    TIMER_scanBase = -1;
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

    if (TIMER_scanBase > 0) {
        TIMER_scanBase--;
    } else if (TIMER_scanBase == 0) {
        // once scanned, keep an eye on any destroyed structures
        scanBase();
        TIMER_scanBase--; // makes it invalid, hence won't do anything
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

void cPlayerBrainScenario::scanBase() {
    // do repairs?
}

void cPlayerBrainScenario::onNotify(const s_GameEvent &event) {
    if (event.entityOwnerID == player->getId()) {
        if (event.eventType == eGameEventType::GAME_EVENT_STRUCTURE_DESTROYED) {
            onMyStructureDestroyed(event);
        } else if (event.eventType == eGameEventType::GAME_EVENT_STRUCTURE_CREATED) {
            OnMyStructureCreated(event);

        }
    }
}

void cPlayerBrainScenario::OnMyStructureCreated(const s_GameEvent &event) {
    // a structure was created, update our baseplan
    cAbstractStructure *pStructure = structure[event.entityID];
    int placedAtCell = pStructure->getCell();
    bool foundExistingStructureInBase = false;
    for (auto & structurePosition : myBase) {
        if (!structurePosition.isDestroyed) continue; // not destroyed, hence cannot be rebuilt

        if (structurePosition.cell == placedAtCell) {
            assert(structurePosition.type == event.entitySpecificType); // should be same structure type...
            // seems my structure, same location.
            structurePosition.structureId = event.entityID;
            structurePosition.isDestroyed = false; // no longer destroyed!
            foundExistingStructureInBase = true;
        }
    }

    if (!foundExistingStructureInBase) {
        char msg[255];
        sprintf(msg, "cPlayerBrainScenario::onNotify() - player [%d / %s] concluded to add structure %s to base register:", player->getId(), player->getHouseName().c_str(), pStructure->getS_StructuresType().name);
        logbook(msg);

        // new structure placed, update base register
        S_structurePosition position = {
                .cell = pStructure->getCell(),
                .type = pStructure->getType(),
                .structureId = pStructure->getStructureId(),
                .isDestroyed = pStructure->isDead()
        };
        myBase.push_back(position);
    }
}

void cPlayerBrainScenario::onMyStructureDestroyed(const s_GameEvent &event) {
    // a structure got destroyed, figure out which one it is in my base plan, and update its state
    for (auto & structurePosition : myBase) {
        if (structurePosition.structureId == event.entityID) {
            // this structure got destroyed, so mark it as destroyed in my base plan
            structurePosition.isDestroyed = true;
            // and add order to rebuild it
            addBuildOrder((S_buildOrder) {
                    buildType : STRUCTURE,
                    priority : 1,
                    buildId : structurePosition.type,
                    placeAt : structurePosition.cell,
                    state : buildOrder::PROCESSME,
            });
        }
    }
}
