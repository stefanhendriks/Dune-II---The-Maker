#include "include/d2tmh.h"
#include "cBuildingListUpdater.h"


cBuildingListUpdater::cBuildingListUpdater(cPlayer *thePlayer) {
	assert(thePlayer);
    player = thePlayer;
}

void cBuildingListUpdater::onStructureCreated(int structureType) {
    logbook("onStructureCreated - begin");

    if (player->isHuman()) {
        // always strict (skirmish) mode. Which means, do not cheat...
        onStructureCreatedSkirmishMode(structureType);
        evaluateUpgrades();
    } else {
        // AI players...

        if (game.bSkirmish) {
            // on skirmish mode use the 'strict' / no cheating mode (same as human players)
            onStructureCreatedSkirmishMode(structureType);
            evaluateUpgrades();
        } else {
            // but on campaign missions, the AI has to cheat in order to be more fun...
            onStructureCreatedCampaignMode(structureType);
            // AI does use upgrades at all...
        }
    }

    // do something
    logbook("onStructureCreated - end");
}

void cBuildingListUpdater::onStructureCreatedCampaignMode(int structureType) const {

    // this is (should be) only called by the AI and in Campaign mode.
    // there the AI only re-builds things. The tech-tree used for players makes no sense if you compare it
    // with the AI. The AI can build anything, even with low power. For example, mission 9 has AI's with low
    // power (yet their turrets should work), but also they don't have all structures to have others (ie, there
    // is a PALACE, but AI players do not have the prerequisites for such a structure).
    //
    // long story short: we give the AI the ability to build *any* structure (and *any* unit, though that is
    // limited to techLevel so the AI can't build things that the player cant unit-wise).

    // activate/deactivate any lists if needed
    cSideBar *sideBar = player->getSideBar();
    cBuildingList *listConstYard = sideBar->getList(LIST_CONSTYARD);
    cBuildingList *listFootUnits = sideBar->getList(LIST_FOOT_UNITS);
    cBuildingList *listUnits = sideBar->getList(LIST_UNITS);

    int house = player->getHouse();
    int techLevel = player->getTechLevel();

    char msg[255];
    sprintf(msg, "For player [%d], structureType [%d], techlevel [%d], house [%d]", player->getId(), structureType, techLevel, house);
    cLogger::getInstance()->log(LOG_INFO, COMP_STRUCTURES, "onStructureCreatedCampaignMode", msg);

    assert(listConstYard);
    assert(listFootUnits);
    assert(listConstYard);

    if (structureType == CONSTYARD) {
        // add items
        listConstYard->addStructureToList(SLAB1, 0);
        listConstYard->addStructureToList(SLAB4, 0);

        // we don't care about techlevel here - since we only rebuild bases anyway...
        listConstYard->addStructureToList(WALL, 0);
        listConstYard->addStructureToList(WINDTRAP, 0);
        listConstYard->addStructureToList(REFINERY, 0);
        listConstYard->addStructureToList(LIGHTFACTORY, 0);
        listConstYard->addStructureToList(BARRACKS, 0);
        listConstYard->addStructureToList(WOR, 0);
        listConstYard->addStructureToList(RADAR, 0);
        listConstYard->addStructureToList(SILO, 0);
        listConstYard->addStructureToList(PALACE, 0);
        listConstYard->addStructureToList(TURRET, 0);
        listConstYard->addStructureToList(RTURRET, 0);
        listConstYard->addStructureToList(IX, 0);
        listConstYard->addStructureToList(HEAVYFACTORY, 0);
        listConstYard->addStructureToList(HIGHTECH, 0);
        listConstYard->addStructureToList(REPAIR, 0);
        listConstYard->addStructureToList(STARPORT, 0);
    }

    if (structureType == LIGHTFACTORY) {
        if (house == ATREIDES) {
            listUnits->addUnitToList(TRIKE, SUBLIST_LIGHTFCTRY);
        } else if (house == ORDOS) {
            listUnits->addUnitToList(RAIDER, SUBLIST_LIGHTFCTRY);
        } else if (house == HARKONNEN) {
            listUnits->addUnitToList(QUAD, SUBLIST_LIGHTFCTRY);
        }

        if (techLevel >= 3) {
            listUnits->addUnitToList(QUAD, SUBLIST_LIGHTFCTRY);
        }
    }

    // Heavyfactory
    if (structureType == HEAVYFACTORY) {
        listUnits->addUnitToList(TANK, SUBLIST_HEAVYFCTRY);
        listUnits->addUnitToList(HARVESTER, SUBLIST_HEAVYFCTRY);
        if (house != ORDOS) {
            listUnits->addUnitToList(LAUNCHER, SUBLIST_HEAVYFCTRY);
        }
        listUnits->addUnitToList(SIEGETANK, SUBLIST_HEAVYFCTRY);
        listUnits->addUnitToList(MCV, SUBLIST_HEAVYFCTRY);
    }

    ///////////////////////////////////
    // ADJUSTMENTS TO INFANTRY LIST
    ///////////////////////////////////
    if (structureType == BARRACKS) {
        listFootUnits->addUnitToList(SOLDIER, SUBLIST_INFANTRY);
        if (techLevel >= 2) {
            listFootUnits->addUnitToList(INFANTRY, SUBLIST_INFANTRY);
        }
    } else if (structureType == WOR) {
        listFootUnits->addUnitToList(TROOPER, SUBLIST_TROOPERS);
        if (techLevel > 2) {
            listFootUnits->addUnitToList(TROOPERS, SUBLIST_TROOPERS);
        }
    }

    ///////////////////////////////////
    // ADJUSTMENTS TO HEAVY FACTORY LIST
    ///////////////////////////////////

    // Heavyfactory
    if (techLevel >= 7) {
        if (house == ATREIDES) {
            listUnits->addUnitToList(SONICTANK, SUBLIST_HEAVYFCTRY);
        } else if (house == HARKONNEN || house == SARDAUKAR) {
            listUnits->addUnitToList(DEVASTATOR, SUBLIST_HEAVYFCTRY);
        } else if (house == ORDOS) {
            listUnits->addUnitToList(DEVIATOR, SUBLIST_HEAVYFCTRY);
        }
    }

    if (structureType == HIGHTECH) {
        listUnits->addUnitToList(CARRYALL, SUBLIST_HIGHTECH);
        if (house == ATREIDES || house == ORDOS) {
            listUnits->addUnitToList(ORNITHOPTER, SUBLIST_HIGHTECH);
        }
    }

    if (structureType == STARPORT) {
        cBuildingList *list = sideBar->getList(LIST_STARPORT);
        list->addUnitToList(INFANTRY, 0);
        list->addUnitToList(TROOPERS, 0);
        list->addUnitToList(TRIKE, 0);
        list->addUnitToList(QUAD, 0);
        list->addUnitToList(TANK, 0);
        list->addUnitToList(MCV, 0);
        list->addUnitToList(HARVESTER, 0);
        list->addUnitToList(LAUNCHER, 0);
        list->addUnitToList(SIEGETANK, 0);
        list->addUnitToList(CARRYALL, 0);
    }

    if (structureType == PALACE) {
        cBuildingList *listPalace = sideBar->getList(LIST_PALACE);
        // special weapons
        switch (house) {
            case ATREIDES:
                listPalace->addSpecialToList(SPECIAL_FREMEN, 0);
                break;
            case HARKONNEN:
                listPalace->addSpecialToList(SPECIAL_DEATHHAND, 0);
                break;
            case SARDAUKAR:
                listPalace->addSpecialToList(SPECIAL_DEATHHAND, 0);
                break;
            case ORDOS:
                listPalace->addSpecialToList(SPECIAL_SABOTEUR, 0);
                break;
        }
    }
}

void cBuildingListUpdater::onStructureCreatedSkirmishMode(int structureType) const {
    // activate/deactivate any lists if needed
    cSideBar *sideBar = player->getSideBar();
    cBuildingList *listConstYard = sideBar->getList(LIST_CONSTYARD);
    cBuildingList *listFootUnits = sideBar->getList(LIST_FOOT_UNITS);
    cBuildingList *listUnits = sideBar->getList(LIST_UNITS);

    int house = player->getHouse();
    int techLevel = player->getTechLevel();

    assert(listConstYard);
    assert(listFootUnits);
    assert(listConstYard);

    if (structureType == CONSTYARD) {
        // add items
        listConstYard->addStructureToList(SLAB1, 0);
        logbook("onStructureCreated - added SLAB1 to list");

        if (techLevel >= 2) {
            if (house == ATREIDES || house == ORDOS) {
                listConstYard->addStructureToList(BARRACKS, 0);
                logbook("onStructureCreated - added BARRACKS to list");
            }
            if (house == HARKONNEN) {
                listConstYard->addStructureToList(WOR, 0);
                logbook("onStructureCreated - added WOR to list");
            }
        }

        if (techLevel >= 4) {
            //list->addItemToList(new cBuildingListItem(SLAB4, structures[SLAB4])); // only available after upgrading
            listConstYard->addStructureToList(WALL, 0);
            logbook("onStructureCreated - added WALL to list");
        }

        listConstYard->addStructureToList(WINDTRAP, 0);
        logbook("onStructureCreated - added WINDTRAP to list");
    }

    if (structureType == WINDTRAP) {
        listConstYard->addStructureToList(REFINERY, 0);
    }

    if (structureType == STARPORT) {
        cBuildingList *list = sideBar->getList(LIST_STARPORT);
        list->addUnitToList(INFANTRY, 0);
        list->addUnitToList(TROOPERS, 0);
        list->addUnitToList(TRIKE, 0);
        list->addUnitToList(QUAD, 0);
        list->addUnitToList(TANK, 0);
        list->addUnitToList(MCV, 0);
        list->addUnitToList(HARVESTER, 0);
        list->addUnitToList(LAUNCHER, 0);
        list->addUnitToList(SIEGETANK, 0);
        list->addUnitToList(CARRYALL, 0);
    }

    if (structureType == REFINERY) {
        if (techLevel >= 2) {
            listConstYard->addStructureToList(LIGHTFACTORY, 0);

            if (house == ATREIDES ||
                house == ORDOS ||
                house == FREMEN) {
                listConstYard->addStructureToList(BARRACKS, 0);

                if (house == ORDOS && techLevel >= 5) {
                    listConstYard->addStructureToList(WOR, 0);
                }
            } else if (
                    house == HARKONNEN ||
                    house == SARDAUKAR ||
                    house == FREMEN ||
                    house == MERCENARY) {
                listConstYard->addStructureToList(WOR, 0);
            }
        }

        if (techLevel >= 3) {
            listConstYard->addStructureToList(RADAR, 0);
        }

        listConstYard->addStructureToList(SILO, 0);
    }


    if (structureType == RADAR) {
        if (techLevel >= 5) {
            listConstYard->addStructureToList(TURRET, 0);
        }

        if (techLevel >= 8) {
            listConstYard->addStructureToList(PALACE, 0);
        }
    }

    if (structureType == STARPORT) {
        // House of IX is available if Starport is built
        if (techLevel >= 7) {
            listConstYard->addStructureToList(IX, 0);
        }
    }

    if (structureType == LIGHTFACTORY) {
        if (techLevel >= 4) {
            listConstYard->addStructureToList(HEAVYFACTORY, 0);
        }

        if (techLevel >= 5) {
            listConstYard->addStructureToList(HIGHTECH, 0);
            listConstYard->addStructureToList(REPAIR, 0);
        }

        if (techLevel >= 6) {
            listConstYard->addStructureToList(STARPORT, 0);
        }

        if (house == ATREIDES) {
            listUnits->addUnitToList(TRIKE, SUBLIST_LIGHTFCTRY);
        } else if (house == ORDOS) {
            listUnits->addUnitToList(RAIDER, SUBLIST_LIGHTFCTRY);
        } else if (house == HARKONNEN) {
            listUnits->addUnitToList(QUAD, SUBLIST_LIGHTFCTRY);
        } else {
            listUnits->addUnitToList(TRIKE, SUBLIST_LIGHTFCTRY);
            listUnits->addUnitToList(RAIDER, SUBLIST_LIGHTFCTRY);
            listUnits->addUnitToList(QUAD, SUBLIST_LIGHTFCTRY);
        }
    }

    // Heavyfactory
    if (structureType == HEAVYFACTORY) {
        listUnits->addUnitToList(TANK, SUBLIST_HEAVYFCTRY);
        listUnits->addUnitToList(HARVESTER, SUBLIST_HEAVYFCTRY);
    }

    ///////////////////////////////////
    // ADJUSTMENTS TO INFANTRY LIST
    ///////////////////////////////////
    if (structureType == BARRACKS) {
        listFootUnits->addUnitToList(SOLDIER, SUBLIST_INFANTRY);
    } else if (structureType == WOR) {
        listFootUnits->addUnitToList(TROOPER, SUBLIST_TROOPERS);
    }

    ///////////////////////////////////
    // ADJUSTMENTS TO HEAVY FACTORY LIST
    ///////////////////////////////////

    // Heavyfactory
    if (techLevel >= 7) {
        if (player->hasAtleastOneStructure(HEAVYFACTORY) &&
            player->hasAtleastOneStructure(IX)) {
            if (player->getHouse() == ATREIDES) {
                listUnits->addUnitToList(SONICTANK, SUBLIST_HEAVYFCTRY);
            } else if (player->getHouse() == HARKONNEN) {
                listUnits->addUnitToList(DEVASTATOR, SUBLIST_HEAVYFCTRY);
            } else if (player->getHouse() == ORDOS) {
                listUnits->addUnitToList(DEVIATOR, SUBLIST_HEAVYFCTRY);
            }
        } else {
            if (player->getHouse() == ATREIDES) {
                listUnits->removeItemFromListByBuildId(SONICTANK);
            } else if (player->getHouse() == HARKONNEN) {
                listUnits->removeItemFromListByBuildId(DEVASTATOR);
            } else if (player->getHouse() == ORDOS) {
                listUnits->removeItemFromListByBuildId(DEVIATOR);
            }
        }
    }

    if (structureType == HIGHTECH) {
        listUnits->addUnitToList(CARRYALL, SUBLIST_HIGHTECH);
    }

    if (structureType == PALACE) {
        cBuildingList *listPalace = sideBar->getList(LIST_PALACE);
        // special weapons
        switch (house) {
            case ATREIDES:
                listPalace->addSpecialToList(SPECIAL_FREMEN, 0);
                break;
            case HARKONNEN:
                listPalace->addSpecialToList(SPECIAL_DEATHHAND, 0);
                break;
            case SARDAUKAR:
                listPalace->addSpecialToList(SPECIAL_DEATHHAND, 0);
                break;
            case ORDOS:
                listPalace->addSpecialToList(SPECIAL_SABOTEUR, 0);
                break;
        }
    }
}

// structure destroyed..
void cBuildingListUpdater::onStructureDestroyed(int structureType) {
    logbook("onStructureDestroyed - begin");

    // activate/deactivate any lists if needed
    cSideBar *sideBar = player->getSideBar();
    cItemBuilder *pItemBuilder = player->getItemBuilder();
    cBuildingList *listConstYard = sideBar->getList(LIST_CONSTYARD);

    if (structureType == STARPORT) {
        if (!player->hasAtleastOneStructure(STARPORT)) {
            listConstYard->removeItemFromListByBuildId(IX);
            cOrderProcesser *pProcesser = player->getOrderProcesser();
            pProcesser->clear();
            cBuildingList *listStarport = sideBar->getList(LIST_STARPORT);
            listStarport->resetTimesOrderedForAllItems();
        }
    }

    if (!player->hasAtleastOneStructure(CONSTYARD)) {
        pItemBuilder->removeItemsFromListType(LIST_CONSTYARD, SUBLIST_CONSTYARD);
        listConstYard->removeAllSublistItems(SUBLIST_CONSTYARD);        
    }

    if (player->isHuman()) {
        // always strict (skirmish) mode. Which means, do not cheat...
        onStructureDestroyedSkirmishMode(structureType);
        evaluateUpgrades();
    } else {
        // AI players...

        if (game.bSkirmish) {
            // on skirmish mode use the 'strict' / no cheating mode (same as human players)
            onStructureDestroyedSkirmishMode(structureType);
            evaluateUpgrades();
        } else {
            // but on campaign missions, the AI has to cheat in order to be more fun...
            onStructureDestroyedCampaignMode(structureType);
            // AI does use upgrades at all...
        }
    }

    logbook("onStructureDestroyed - end");
}


void cBuildingListUpdater::evaluateUpgrades() {
    logbook("evaluateUpgrades - start");
    cSideBar *sideBar = player->getSideBar();
    cBuildingList *listUpgrades = sideBar->getList(LIST_UPGRADES);

    for (int i = 0; i < MAX_UPGRADETYPES; i++) {
        s_Upgrade &upgrade = upgrades[i];
        if (!upgrade.enabled) continue;
        // check techlevel (this is a non-changing value per mission, usually coupled with mission nr, ie
        // mission 1 = techlevel 1. Mission 9 = techlevel 9. Skirmish is usually techlevel 9.
        if (player->getTechLevel() < upgrade.techLevel) continue;

        if (!(upgrade.house & player->getHouseBitFlag())) {
            // house specific upgrade, player house does not match
            char msg[255];
            sprintf(msg, "Upgrade [%s] has not same house.", upgrade.description);
            logbook(msg);
            continue;
        }

        bool addToUpgradesList = true;

        // check if player has structure to upgrade
        bool hasAtLeastOneStructureForStructureType = player->hasAtleastOneStructure(upgrade.structureType);
        if (!hasAtLeastOneStructureForStructureType) {
            addToUpgradesList = false;
            char msg[255];
            sprintf(msg, "Upgrade [%s] has not required structureType (upgrade.structureType) #1 [%s].", upgrade.description, structures[upgrade.structureType].name);
            logbook(msg);
        }

        // check if player has the additional structure (if required)
        if (upgrade.needsStructureType > -1) {
            bool hasAtleastOneNeedStructureType = player->hasAtleastOneStructure(upgrade.needsStructureType);
            if (!hasAtleastOneNeedStructureType) {
                addToUpgradesList = false;
                char msg[255];
                sprintf(msg, "Upgrade [%s] has not required additional structureType (upgrade.needsStructureType) [%s].", upgrade.description, structures[upgrade.needsStructureType].name);
                logbook(msg);
            }
        }

        // check if the structure to upgrade is at the expected level
        int structureUpgradeLevel = player->getStructureUpgradeLevel(upgrade.structureType);

        if (structureUpgradeLevel != upgrade.atUpgradeLevel) {
            // already at correct upgrade level, no = comparison because that means it should be offered
            if (structureUpgradeLevel > upgrade.atUpgradeLevel) {
                // this upgrade has been executed already, so apply again
                applyUpgrade(upgrade);
                char msg[255];
                sprintf(msg, "Upgrade [%s] has already been achieved, so re-apply. StructureUpgradeLevel=%d and upgrade.atUpgradeLevel=%d.", upgrade.description, structureUpgradeLevel, upgrade.atUpgradeLevel);
                logbook(msg);
            }
            addToUpgradesList = false;
            char msg[255];
            sprintf(msg, "Upgrade [%s] will not be offered because it has a different atUpgradeLevel. StructureUpgradeLevel=%d and upgrade.atUpgradeLevel=%d not required additional structureType (upgrade.needsStructureType).", upgrade.description, structureUpgradeLevel, upgrade.atUpgradeLevel);
            logbook(msg);
        }

        if (addToUpgradesList) {
            listUpgrades->addUpgradeToList(i);
        } else {
            cBuildingListItem * item = listUpgrades->getItemByBuildId(i);

            if (item) {
                if (item->isBuilding() && !item->isDoneBuilding()) {
                    // DUPLICATION OF cSidebar::thinkInteraction, line ~ 195!
                    cLogger::getInstance()->log(LOG_INFO, COMP_BUILDING_LIST_UPDATER, "Cancel upgrading", "Upgrade no longer available, aborted mid-way upgrading so refunding.");
                    // only give money back for item that is being built
                    if (item->isBuilding()) {
                        // calculate the amount of money back:
                        player->giveCredits(item->getRefundAmount());
                    }
                    item->setIsBuilding(false);
                    item->resetProgress();
                    cItemBuilder *itemBuilder = player->getItemBuilder();
                    itemBuilder->removeItemFromList(item);
                }
            }

            // remove it
            listUpgrades->removeItemFromListByBuildId(i);
        }
    }

    logbook("evaluateUpgrades - end");
}

/**
 * method called, when buildingListItem (the upgrade) has finished building.
 */
void cBuildingListUpdater::onUpgradeCompleted(cBuildingListItem *item) {
	assert(item);
    if (!item->isTypeUpgrade()) {
        logbook("ERROR ERROR ERROR! -> the provided item is NOT an upgrade type!");
        assert(false);
        return;
    }

    const s_Upgrade &upgradeType = item->getS_Upgrade();

    // Upgrade structure + provide any unit or structure
    player->increaseStructureUpgradeLevel(upgradeType.structureType);

    applyUpgrade(upgradeType);

    evaluateUpgrades();
}


/**
 * This function will 'apply' the given upgrade (build item must be upgrade type). This is executed when
 * an upgrade is completed OR when upgrades are re-evaluated after structure placement/destroying and thus
 * needs to be re-applied.
 * @param item
 */
void cBuildingListUpdater::applyUpgrade(const s_Upgrade &upgradeType) {
    cSideBar *sideBar = player->getSideBar();
    int listType = upgradeType.providesTypeList;
    int subListType = upgradeType.providesTypeSubList;

    cBuildingList *listBeingUpgraded = sideBar->getList(listType);
    listBeingUpgraded->setStatusAvailable(subListType);
    
    assert(upgradeType.providesTypeId > -1);
    assert(upgradeType.providesType > -1);

    assert(listType > -1);
    assert(listType < LIST_UPGRADES);

    cBuildingList *list = sideBar->getList(listType);
    if (upgradeType.providesType == UNIT) {
        list->addUnitToList(upgradeType.providesTypeId, subListType);
    } else if (upgradeType.providesType == STRUCTURE) {
        list->addStructureToList(upgradeType.providesTypeId, subListType);
    }
}

/**
 * Called when a BuildingLisItem is being started (to build), and it is of type
 * upgrade.
 *
 * This should influence the corresponding list/sublist.
 * @param pItem
 */
void cBuildingListUpdater::onUpgradeStarted(cBuildingListItem *pItem) {
    assert(pItem);
    assert(pItem->isTypeUpgrade());
    cSideBar *sideBar = player->getSideBar();

    // get the structure type it is upgrading
    const s_Upgrade &upgrade = pItem->getS_Upgrade();
    int listType = upgrade.providesTypeList;
    int subListType = upgrade.providesTypeSubList;

    cBuildingList *listBeingUpgraded = sideBar->getList(listType);
    listBeingUpgraded->setStatusPendingUpgrade(subListType);
}

/**
 * Called when a BuildingListItem is being cancelled (while building), and it is of type
 * upgrade.
 * @param pItem
 */
void cBuildingListUpdater::onUpgradeCancelled(cBuildingListItem *pItem) {
    cLogger::getInstance()->logCommentLine("onUpgradeCancelled - start");
    assert(pItem);
    assert(pItem->isTypeUpgrade());

    cSideBar *sideBar = player->getSideBar();

    // get the structure type it is upgrading
    const s_Upgrade &upgrade = pItem->getS_Upgrade();
    int listType = upgrade.providesTypeList;
    int subListType = upgrade.providesTypeSubList;

    cBuildingList *listBeingUpgraded = sideBar->getList(listType);
    listBeingUpgraded->setStatusAvailable(subListType);

    cLogger::getInstance()->logCommentLine("onUpgradeCancelled - end");
}

void cBuildingListUpdater::onBuildItemCancelled(cBuildingListItem *pItem) {
    cLogger::getInstance()->logCommentLine("onBuildItemCancelled - start");
    if (pItem == nullptr) return;
    if (pItem->isTypeUpgrade()) {
        onUpgradeCancelled(pItem);
        return;
    }

    // it is a unit/structure/special
    cSideBar *sideBar = player->getSideBar();
    cBuildingList *listUpgrades = sideBar->getList(LIST_UPGRADES);
    listUpgrades->setStatusAvailable(pItem->getSubList());

    cLogger::getInstance()->logCommentLine("onBuildItemCancelled - end");
}

/**
 * Called when a buildItem is started to build/upgrade (by cItemBuilder).
 * @param pItem
 */
void cBuildingListUpdater::onBuildItemStarted(cBuildingListItem *pItem) {
    if (pItem == nullptr) return;
    if (pItem->isTypeUpgrade()) {
        onUpgradeStarted(pItem);
        return;
    }

    if (pItem->isTypeSpecial()) {
        // do nothing
        return;
    }

    // it is a unit/structure/special
    cSideBar *sideBar = player->getSideBar();
    cBuildingList *listUpgrades = sideBar->getList(LIST_UPGRADES);
    listUpgrades->setStatusPendingBuilding(pItem->getSubList());
}

/**
 * A unit is done building; a super weapon is completed
 * @param pItem
 */
void cBuildingListUpdater::onBuildItemCompleted(cBuildingListItem *pItem) {
    if (pItem == nullptr) return;
    if (pItem->isTypeUpgrade()) {
        // do nothing, this is normally called via OnUpgradeCompleted, see method above.
        logbook("Wrongfully called onBuildItemCompleted when OnUpgradeCompleted was expected.");
        return;
    }

    if (pItem->isTypeSpecial()) {
        // do stuff here for special item
        const s_Special &special = pItem->getS_Special();
        return;
    }

    // it is a unit/structure/special
    cSideBar *sideBar = player->getSideBar();
    cBuildingList *listUpgrades = sideBar->getList(LIST_UPGRADES);
    listUpgrades->setStatusAvailable(pItem->getSubList());
}

void cBuildingListUpdater::onStructureDestroyedSkirmishMode(int structureType) const {
    cSideBar *sideBar = player->getSideBar();
    cItemBuilder *pItemBuilder = player->getItemBuilder();
    cBuildingList *listUnits = sideBar->getList(LIST_UNITS);

    int house = player->getHouse();
    int techLevel = player->getTechLevel();

    if (techLevel >= 7) {
        if (!player->hasAtleastOneStructure(HEAVYFACTORY) ||
            !player->hasAtleastOneStructure(IX)) {
            if (player->getHouse() == ATREIDES) {
                pItemBuilder->removeItemsByBuildId(UNIT, SONICTANK);
                listUnits->removeItemFromListByBuildId(SONICTANK);
            } else if (player->getHouse() == HARKONNEN) {
                pItemBuilder->removeItemsByBuildId(UNIT, DEVASTATOR);
                listUnits->removeItemFromListByBuildId(DEVASTATOR);
            } else if (player->getHouse() == ORDOS) {
                pItemBuilder->removeItemsByBuildId(UNIT, DEVIATOR);
                listUnits->removeItemFromListByBuildId(DEVIATOR);
            }
        }
    }

    if (!player->hasAtleastOneStructure(HIGHTECH)) {
        pItemBuilder->removeItemsFromListType(LIST_UNITS, SUBLIST_HIGHTECH);
        listUnits->removeAllSublistItems(SUBLIST_HIGHTECH);
    }

    if (!player->hasAtleastOneStructure(LIGHTFACTORY)) {
        pItemBuilder->removeItemsFromListType(LIST_UNITS, SUBLIST_LIGHTFCTRY);
        listUnits->removeAllSublistItems(SUBLIST_LIGHTFCTRY);
    }

    if (!player->hasAtleastOneStructure(HEAVYFACTORY)) {
        pItemBuilder->removeItemsFromListType(LIST_UNITS, SUBLIST_HEAVYFCTRY);
        listUnits->removeAllSublistItems(SUBLIST_HEAVYFCTRY);
    }

    if (!player->hasAtleastOneStructure(PALACE)) {
        pItemBuilder->removeItemsFromListType(LIST_PALACE, 0);
        listUnits->removeAllSublistItems(SUBLIST_HEAVYFCTRY);
    }
}

void cBuildingListUpdater::onStructureDestroyedCampaignMode(int structureType) const {
    cSideBar *sideBar = player->getSideBar();
    cItemBuilder *pItemBuilder = player->getItemBuilder();
    cBuildingList *listUnits = sideBar->getList(LIST_UNITS);

    int house = player->getHouse();
    int techLevel = player->getTechLevel();

    if (!player->hasAtleastOneStructure(HIGHTECH)) {
        pItemBuilder->removeItemsFromListType(LIST_UNITS, SUBLIST_HIGHTECH);
        listUnits->removeAllSublistItems(SUBLIST_HIGHTECH);
    }

    if (!player->hasAtleastOneStructure(LIGHTFACTORY)) {
        pItemBuilder->removeItemsFromListType(LIST_UNITS, SUBLIST_LIGHTFCTRY);
        listUnits->removeAllSublistItems(SUBLIST_LIGHTFCTRY);
    }

    if (!player->hasAtleastOneStructure(HEAVYFACTORY)) {
        pItemBuilder->removeItemsFromListType(LIST_UNITS, SUBLIST_HEAVYFCTRY);
        listUnits->removeAllSublistItems(SUBLIST_HEAVYFCTRY);
    }

    if (!player->hasAtleastOneStructure(PALACE)) {
        pItemBuilder->removeItemsFromListType(LIST_PALACE, 0);
        listUnits->removeAllSublistItems(SUBLIST_HEAVYFCTRY);
    }
}
