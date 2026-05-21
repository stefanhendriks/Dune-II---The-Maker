#include "game/cGameEventHandler.h"

#include "building/cItemBuilder.h"
#include "context/cGameObjectContext.h"
#include "context/cInfoContext.h"
#include "data/gfxaudio.h"
#include "definitions.h"
#include "controls/cGameControlsContext.h"
#include "game/cGameInterface.h"
#include "gameobjects/map/MapGeometry.hpp"
#include "gameobjects/map/cMap.h"
#include "gameobjects/players/cPlayer.h"
#include "gameobjects/players/cPlayers.h"
#include "gameobjects/structures/cStructureInfo.h"
#include "gameobjects/structures/cStructures.h"
#include "gameobjects/units/cUnits.h"
#include "sidebar/cBuildingListItem.h"
#include "utils/RNG.hpp"
#include "utils/cStructureUtils.h"
#include "utils/common.h"
#include "utils/cLog.h"

#include <format>

cGameEventHandler::cGameEventHandler(
    cGameObjectContext *gameObjectsContext,
    cInfoContext *infoContext,
    cStructureUtils *structureUtils,
    cGameInterface *gameInterface
) : m_gameObjectsContext(gameObjectsContext),
    m_infoContext(infoContext),
    m_structureUtils(structureUtils),
    m_gameInterface(gameInterface) {
}

void cGameEventHandler::handleEvent(const s_GameEvent &event)
{
    logbook(s_GameEvent::toString(event));

    m_gameObjectsContext->getMap()->onNotifyGameEvent(event);

    switch (event.eventType) {
        case eGameEventType::GAME_EVENT_SPECIAL_LAUNCH:
            if (const auto *launchEvent = std::get_if<LaunchDeathHandEvent>(&event.data)) {
                onEventSpecialLaunch(*launchEvent);
            }
            break;
        case eGameEventType::GAME_EVENT_DESTROYED:
            if (const auto *destroyEvent = std::get_if<CommonEvent>(&event.data)) {
                onEventEntityDestroyed(*destroyEvent);
            }
            break;
        case eGameEventType::GAME_EVENT_CREATE_UNIT:
            if (const auto *deployEvent = std::get_if<DeployUnitEvent>(&event.data)) {
                onEventCreateUnit(*deployEvent);
            }
            break;
        default:
            break;
    }

    if (event.eventType == eGameEventType::GAME_EVENT_NOTIFICATION) {
        if (const auto *notifEvent = std::get_if<NotificationEvent>(&event.data)) {
            m_gameInterface->addNotification(notifEvent->message, notifEvent->type);
        }
    }

    m_gameObjectsContext->getPlayers()->onNotifyGameEvent(event);
}

void cGameEventHandler::onEventEntityDestroyed(const CommonEvent &event)
{
    if (event.entityType != eBuildType::STRUCTURE) {
        return;
    }

    const auto structureInfo = (*m_infoContext->getStructureInfos())[event.entitySpecificType];
    int unitTypeToSpawn = structureInfo.uponDestructionSpawnUnitType;
    if (unitTypeToSpawn < 0) {
        return;
    }

    int minAmountOfUnitsToSpawnPotentially = structureInfo.uponDestructionSpawnUnitAmountMin;
    int maxAmountOfUnitsToSpawnPotentially = structureInfo.uponDestructionSpawnUnitAmountMax;

    int amountOfUnitsToSpawn = RNG::genInt(minAmountOfUnitsToSpawnPotentially, maxAmountOfUnitsToSpawnPotentially);

    int widthInCells = structureInfo.bmp_width / 32;
    int heightInCells = structureInfo.bmp_height / 32;

    int cellX = m_gameObjectsContext->getMapGeometry()->getCellX(event.atCell);
    int cellY = m_gameObjectsContext->getMapGeometry()->getCellY(event.atCell);

    for (int i = 0; i < amountOfUnitsToSpawn; i++) {
        int randomX = cellX + RNG::genIntMaxExcl(0, widthInCells);
        int randomY = cellY + RNG::genIntMaxExcl(0, heightInCells);
        cUnits::unitCreate(
            m_gameObjectsContext->getMapGeometry()->makeCell(randomX, randomY),
            unitTypeToSpawn,
            event.player->getId(),
            false,
            false,
            RNG::genDouble(0.3, 0.8)
        );
    }
}

void cGameEventHandler::onEventCreateUnit(const DeployUnitEvent &event)
{
    if (event.iCell < 0 || event.unitType < 0 || event.iPlayer < 0) {
        return;
    }
    int id = cUnits::unitCreate(
        event.iCell,
        event.unitType,
        event.iPlayer,
        event.bOnStart,
        event.isReinforcement,
        event.hpPercentage
    );

    if (id < 0) {
        cLogger::getInstance()->log(LOG_ERROR, COMP_GAME, "Deploy unit",
                                    std::format("Failed to deploy unit of type {} at cell {} for player {}", event.unitType, event.iCell, event.iPlayer)
        );
    }
    else {
        cLogger::getInstance()->log(LOG_INFO, COMP_GAME, "Deploy unit",
                                    std::format("Successfully deployed unit of type {} at cell {} for player {}, id={}", event.unitType, event.iCell, event.iPlayer, id)
        );
    }
}

void cGameEventHandler::onEventSpecialLaunch(const LaunchDeathHandEvent &event)
{
    cBuildingListItem *itemToDeploy = event.itemToLaunch;
    int iMouseCell = event.targetCell;
    cPlayer *player = event.player;

    if (itemToDeploy->isTypeSpecial()) {
        const s_SpecialInfo &special = itemToDeploy->getSpecialInfo();

        int deployCell = -1;
        if (special.deployTargetType == eDeployTargetType::TARGET_SPECIFIC_CELL) {
            deployCell = iMouseCell;
        }
        else if (special.deployTargetType == eDeployTargetType::TARGET_INACCURATE_CELL) {
            int precision = special.deployTargetPrecision;
            int mouseCellX = m_gameObjectsContext->getMapGeometry()->getCellX(iMouseCell) - precision;
            int mouseCellY = m_gameObjectsContext->getMapGeometry()->getCellY(iMouseCell) - precision;

            int posX = mouseCellX + RNG::rnd((precision * 2) + 1);
            int posY = mouseCellY + RNG::rnd((precision * 2) + 1);
            cPoint::split(posX, posY) = m_gameObjectsContext->getMapGeometry()->fixCoordinatesToBeWithinPlayableMap(posX, posY);

            logbook(std::format(
                "eDeployTargetType::TARGET_INACCURATE_CELL, mouse cell X,Y = {},{} - target pos ={},{} - precision {}",
                mouseCellY, mouseCellY, posX, posY, precision)
            );

            deployCell = m_gameObjectsContext->getMapGeometry()->makeCell(posX, posY);
        }

        if (special.providesType == eBuildType::BULLET) {
            int structureId = m_structureUtils->findStructureBy(player->getId(), special.deployAtStructure, false);
            if (structureId > -1) {
                cAbstractStructure *pStructure = m_gameObjectsContext->getStructures()[structureId];
                if (pStructure && pStructure->isValid()) {
                    m_gameInterface->playSound(SOUND_PLACE);
                    createBullet(special.providesTypeId, pStructure->getCell(), deployCell, -1, structureId);

                    const s_GameEvent launchedEvent{
                        .eventType = eGameEventType::GAME_EVENT_SPECIAL_LAUNCHED,
                        .data = BuildingEvent{
                            .entityType = itemToDeploy->getBuildType(),
                            .player = pStructure->getPlayer(),
                            .entitySpecificType = itemToDeploy->getBuildId(),
                            .buildingListItem = itemToDeploy
                        }
                    };
                    m_gameInterface->onNotifyGameEvent(launchedEvent);
                }
            }
        }
    }

    itemToDeploy->decreaseTimesToBuild();
    itemToDeploy->setDeployIt(false);
    itemToDeploy->cancelBuilding();
    if (itemToDeploy->getTimesToBuild() < 1) {
        player->getItemBuilder()->removeItemFromList(itemToDeploy);
    }

    if (player) {
        player->getGameControlsContext()->toPreviousState();
    }

    const s_GameEvent finishedEvent{
        .eventType = eGameEventType::GAME_EVENT_LIST_ITEM_FINISHED,
        .data = BuildingEvent{
            .entityType = itemToDeploy->getBuildType(),
            .player = player,
            .entitySpecificType = itemToDeploy->getBuildId()
        }
    };
    m_gameInterface->onNotifyGameEvent(finishedEvent);
}
