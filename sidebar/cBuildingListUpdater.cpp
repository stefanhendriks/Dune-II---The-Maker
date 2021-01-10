#include "include/d2tmh.h"

cBuildingListUpdater::cBuildingListUpdater(cPlayer *thePlayer) {
	assert(thePlayer);
    m_Player = thePlayer;
}

void cBuildingListUpdater::onStructureCreated(int structureType) {
    logbook("onStructureCreated - begin");

	// activate/deactivate any lists if needed
    cSideBar *sideBar = m_Player->getSideBar();
    cBuildingList *listConstYard = sideBar->getList(LIST_CONSTYARD);
    cBuildingList *listFootUnits = sideBar->getList(LIST_FOOT_UNITS);
    cBuildingList *listUnits = sideBar->getList(LIST_UNITS);

	int house = m_Player->getHouse();
	int techLevel = m_Player->getTechLevel();

//    char msg[255];
//    sprintf(msg, "onStructureCreated - for player [%d], structureType [%d], techlevel [%d], house [%d]", m_Player->getId(), structureType, techLevel, house);
//    cLogger::getInstance()->log(LOG_INFO, COMP_STRUCTURES, "onStructureCreated", msg);

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
			listConstYard->addStructureToList(LIGHTFACTORY,0);

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

	if (structureType == LIGHTFACTORY)
	{
		if (techLevel >=4) {
			listConstYard->addStructureToList(HEAVYFACTORY, 0);
		}

		if (techLevel >=5) {
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
        if (m_Player->hasAtleastOneStructure(HEAVYFACTORY) &&
            m_Player->hasAtleastOneStructure(IX)) {
            if (m_Player->getHouse() == ATREIDES) {
                listUnits->addUnitToList(SONICTANK, SUBLIST_HEAVYFCTRY);
            } else if (m_Player->getHouse() == HARKONNEN) {
                listUnits->addUnitToList(DEVASTATOR, SUBLIST_HEAVYFCTRY);
            } else if (m_Player->getHouse() == ORDOS) {
                listUnits->addUnitToList(DEVIATOR, SUBLIST_HEAVYFCTRY);
            }
        } else {
            if (m_Player->getHouse() == ATREIDES) {
                listUnits->removeItemFromListByBuildId(SONICTANK);
            } else if (m_Player->getHouse() == HARKONNEN) {
                listUnits->removeItemFromListByBuildId(DEVASTATOR);
            } else if (m_Player->getHouse() == ORDOS) {
                listUnits->removeItemFromListByBuildId(DEVIATOR);
            }
        }
    }

	if (structureType == HIGHTECH) {
        listUnits->addUnitToList(CARRYALL, SUBLIST_HIGHTECH);
    }

    evaluateUpgrades();

    // do something
    logbook("onStructureCreated - end");
}

// structure destroyed..
void cBuildingListUpdater::onStructureDestroyed(int structureType) {
    logbook("onStructureDestroyed - begin");

    // activate/deactivate any lists if needed
    cSideBar *sideBar = m_Player->getSideBar();
    cBuildingList *listConstYard = sideBar->getList(LIST_CONSTYARD);
    cBuildingList *listFootUnits = sideBar->getList(LIST_FOOT_UNITS);
    cBuildingList *listUnits = sideBar->getList(LIST_UNITS);

    if (structureType == STARPORT) {
        if (!m_Player->hasAtleastOneStructure(STARPORT)) {
            listConstYard->removeItemFromListByBuildId(IX);
            cOrderProcesser *pProcesser = m_Player->getOrderProcesser();
            pProcesser->clear();
            cBuildingList *listStarport = sideBar->getList(LIST_STARPORT);
            listStarport->resetTimesOrderedForAllItems();
        }
    }

    // do something
    int house = m_Player->getHouse();
    int techLevel = m_Player->getTechLevel();

    if (!m_Player->hasAtleastOneStructure(CONSTYARD)) {
        listConstYard->removeAllSublistItems(SUBLIST_CONSTYARD);
    }

    if (techLevel >= 7) {
        if (!m_Player->hasAtleastOneStructure(HEAVYFACTORY) ||
            !m_Player->hasAtleastOneStructure(IX)) {
            if (m_Player->getHouse() == ATREIDES) {
                listUnits->removeItemFromListByBuildId(SONICTANK);
            } else if (m_Player->getHouse() == HARKONNEN) {
                listUnits->removeItemFromListByBuildId(DEVASTATOR);
            } else if (m_Player->getHouse() == ORDOS) {
                listUnits->removeItemFromListByBuildId(DEVIATOR);
            }
        }
    }

    if (!m_Player->hasAtleastOneStructure(HIGHTECH)) {
        listUnits->removeAllSublistItems(SUBLIST_HIGHTECH);
    }

    if (!m_Player->hasAtleastOneStructure(LIGHTFACTORY)) {
        listUnits->removeAllSublistItems(SUBLIST_LIGHTFCTRY);
    }

    if (!m_Player->hasAtleastOneStructure(HEAVYFACTORY)) {
        listUnits->removeAllSublistItems(SUBLIST_HEAVYFCTRY);
    }

//    char msg[255];
//    sprintf(msg, "onStructureDestroyed - for player [%d], structureType [%d], techlevel [%d], house [%d]", m_Player->getId(), structureType, techLevel, house);
//    cLogger::getInstance()->log(LOG_INFO, COMP_STRUCTURES, "onStructureDestroyed", msg);

    evaluateUpgrades();

    logbook("onStructureDestroyed - end");
}


void cBuildingListUpdater::evaluateUpgrades() {
    logbook("evaluateUpgrades - start");
    cSideBar *sideBar = m_Player->getSideBar();
    cBuildingList *listUpgrades = sideBar->getList(LIST_UPGRADES);

    for (int i = 0; i < MAX_UPGRADETYPES; i++) {
        s_Upgrade &upgrade = upgrades[i];
        if (!upgrade.enabled) continue;
        // check techlevel (this is a non-changing value per mission, usually coupled with mission nr, ie
        // mission 1 = techlevel 1. Mission 9 = techlevel 9. Skirmish is usually techlevel 9.
        if (m_Player->getTechLevel() < upgrade.techLevel) continue;

        if (!(upgrade.house & m_Player->getHouseBitFlag())) {
            // house specific upgrade, player house does not match
            char msg[255];
            sprintf(msg, "Upgrade [%s] has not same house.", upgrade.description);
            logbook(msg);
            continue;
        }

        bool addToUpgradesList = true;

        // check if player has structure to upgrade
        bool hasAtLeastOneStructureForStructureType = m_Player->hasAtleastOneStructure(upgrade.structureType);
        if (!hasAtLeastOneStructureForStructureType) {
            addToUpgradesList = false;
            char msg[255];
            sprintf(msg, "Upgrade [%s] has not required structureType (upgrade.structureType) #1 [%s].", upgrade.description, structures[upgrade.structureType].name);
            logbook(msg);
        }

        // check if player has the additional structure (if required)
        if (upgrade.needsStructureType > -1) {
            bool hasAtleastOneNeedStructureType = m_Player->hasAtleastOneStructure(upgrade.needsStructureType);
            if (!hasAtleastOneNeedStructureType) {
                addToUpgradesList = false;
                char msg[255];
                sprintf(msg, "Upgrade [%s] has not required additional structureType (upgrade.needsStructureType) [%s].", upgrade.description, structures[upgrade.needsStructureType].name);
                logbook(msg);
            }
        }

        // check if the structure to upgrade is at the expected level
        int structureUpgradeLevel = m_Player->getStructureUpgradeLevel(upgrade.structureType);

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
                        m_Player->credits += item->getRefundAmount();
                    }
                    item->setIsBuilding(false);
                    item->setProgress(0);
                    cItemBuilder *itemBuilder = m_Player->getItemBuilder();
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
    m_Player->increaseStructureUpgradeLevel(upgradeType.structureType);

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
    cSideBar *sideBar = m_Player->getSideBar();
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
    cSideBar *sideBar = m_Player->getSideBar();

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

    cSideBar *sideBar = m_Player->getSideBar();

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
    cSideBar *sideBar = m_Player->getSideBar();
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

    // it is a unit/structure/special
    cSideBar *sideBar = m_Player->getSideBar();
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

    // it is a unit/structure/special
    cSideBar *sideBar = m_Player->getSideBar();
    cBuildingList *listUpgrades = sideBar->getList(LIST_UPGRADES);
    listUpgrades->setStatusAvailable(pItem->getSubList());
}