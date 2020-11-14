/*
 * cBuildingListUpgrader.cpp
 *
 *  Created on: Aug 1, 2009
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cBuildingListUpdater::cBuildingListUpdater(cPlayer *thePlayer) {
	assert(thePlayer);
	player = thePlayer;
}

void cBuildingListUpdater::onStructureCreated(int structureType) {
	cLogger::getInstance()->logCommentLine("onStructureCreated - begin");

	// activate/deactivate any lists if needed
    cSideBar *sideBar = player->getSideBar();
    cBuildingList *listConstYard = sideBar->getList(LIST_CONSTYARD);
    cBuildingList *listFootUnits = sideBar->getList(LIST_FOOT_UNITS);
    cBuildingList *listUnits = sideBar->getList(LIST_UNITS);

	int house = player->getHouse();
	int techLevel = player->getTechLevel();

    char msg[255];
    sprintf(msg, "onStructureCreated - for player [%d], structureType [%d], techlevel [%d], house [%d]", player->getId(), structureType, techLevel, house);
    cLogger::getInstance()->log(LOG_INFO, COMP_STRUCTURES, "onStructureCreated", msg);

	assert(listConstYard);
	assert(listFootUnits);
	assert(listConstYard);

	if (structureType == CONSTYARD) {
        // add items
        listConstYard->addStructureToList(SLAB1, 0);
        cLogger::getInstance()->logCommentLine("onStructureCreated - added SLAB1 to list");

        if (techLevel >= 2) {
            if (house == ATREIDES || house == ORDOS) {
                listConstYard->addStructureToList(BARRACKS, 0);
                cLogger::getInstance()->logCommentLine("onStructureCreated - added BARRACKS to list");
            }
            if (house == HARKONNEN) {
                listConstYard->addStructureToList(WOR, 0);
                cLogger::getInstance()->logCommentLine("onStructureCreated - added WOR to list");
            }
        }

        if (techLevel >= 4) {
            //list->addItemToList(new cBuildingListItem(SLAB4, structures[SLAB4])); // only available after upgrading
            listConstYard->addStructureToList(WALL, 0);
            cLogger::getInstance()->logCommentLine("onStructureCreated - added WALL to list");
        }

        listConstYard->addStructureToList(WINDTRAP, 0);
        cLogger::getInstance()->logCommentLine("onStructureCreated - added WINDTRAP to list");
    }

	if (structureType == WINDTRAP) {
		listConstYard->addStructureToList(REFINERY, 0);
		cLogger::getInstance()->logCommentLine("onStructureCreated - added REFINARY to list");
	}

	if (structureType == REFINERY) {
		if (techLevel >= 2) {
			cLogger::getInstance()->logCommentLine("onStructureCreated - added LIGHTFACTORY to list");
			listConstYard->addStructureToList(LIGHTFACTORY,0);

			if (house == ATREIDES ||
				house == ORDOS ||
				house == FREMEN) {
				cLogger::getInstance()->logCommentLine("onStructureCreated - added BARRACKS to list");
				listConstYard->addStructureToList(BARRACKS, 0);

				if (house == ORDOS && techLevel >= 5) {
					cLogger::getInstance()->logCommentLine("onStructureCreated - added WOR to list");
					listConstYard->addStructureToList(WOR, 0);
				}
			} else if (
					house == HARKONNEN ||
					house == SARDAUKAR ||
					house == FREMEN ||
					house == MERCENARY) {
				cLogger::getInstance()->logCommentLine("onStructureCreated - added WOR to list");
				listConstYard->addStructureToList(WOR, 0);
			}
		}

		if (techLevel >= 3) {
			listConstYard->addStructureToList(RADAR, 0);
		}

		cLogger::getInstance()->logCommentLine("onStructureCreated - added SILO to list");
		listConstYard->addStructureToList(SILO, 0);
	}


	if (structureType == RADAR) {
        if (techLevel >= 5) {
			cLogger::getInstance()->logCommentLine("onStructureCreated - added TURRET to list");
			listConstYard->addStructureToList(TURRET, 0);
		}

		if (techLevel >= 8) {
			cLogger::getInstance()->logCommentLine("onStructureCreated - added PALACE to list");
			listConstYard->addStructureToList(PALACE, 0);
		}
	}

	if (structureType == LIGHTFACTORY)
	{
		if (techLevel >=4) {
			cLogger::getInstance()->logCommentLine("onStructureCreated - added HEAVYFACTORY to list");
			listConstYard->addStructureToList(HEAVYFACTORY, 0);
		}

		if (techLevel >=5) {
			cLogger::getInstance()->logCommentLine("onStructureCreated - added HIGHTECH to list");
			cLogger::getInstance()->logCommentLine("onStructureCreated - added REPAIR to list");
			listConstYard->addStructureToList(HIGHTECH, 0);
			listConstYard->addStructureToList(REPAIR, 0);
		}

		if (techLevel >= 6) {
			cLogger::getInstance()->logCommentLine("onStructureCreated - added STARPORT to list");
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
	if (structureType == HEAVYFACTORY)
	{
		if (techLevel >= 7) {
			cLogger::getInstance()->logCommentLine("onStructureCreated - added IX to list");
			listConstYard->addStructureToList(IX, 0);
		}

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
	if (structureType == IX) {
		if (techLevel >= 7) {
			if (player->getHouse() == ATREIDES) {
                listUnits->addUnitToList(SONICTANK, SUBLIST_HEAVYFCTRY);
			} else if (player->getHouse() == HARKONNEN) {
                listUnits->addUnitToList(DEVASTATOR, SUBLIST_HEAVYFCTRY);
			} else if (player->getHouse() == ORDOS) {
                listUnits->addUnitToList(DEVIATOR, SUBLIST_HEAVYFCTRY);
			}
		}
	}

	if (structureType == HIGHTECH) {
        listUnits->addUnitToList(CARRYALL, SUBLIST_HIGHTECH);
    }

    evaluateUpgrades();

    // do something
	cLogger::getInstance()->logCommentLine("onStructureCreated - end");
}

// structure destroyed..
void cBuildingListUpdater::onStructureDestroyed(int structureType) {
	cLogger::getInstance()->logCommentLine("onStructureDestroyed - begin");

	// do something
    int house = player->getHouse();
    int techLevel = player->getTechLevel();

    char msg[255];
    sprintf(msg, "onStructureDestroyed - for player [%d], structureType [%d], techlevel [%d], house [%d]", player->getId(), structureType, techLevel, house);
    cLogger::getInstance()->log(LOG_INFO, COMP_STRUCTURES, "onStructureDestroyed", msg);

    evaluateUpgrades();

    cLogger::getInstance()->logCommentLine("onStructureDestroyed - end");
}


void cBuildingListUpdater::evaluateUpgrades() {
    cLogger::getInstance()->logCommentLine("evaluateUpgrades - start");
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

        bool meetsConditions = true;

        // check if player has structure to upgrade
        bool hasAtLeastOneStructureForStructureType = player->hasAtleastOneStructure(upgrade.structureType);
        if (!hasAtLeastOneStructureForStructureType) {
            meetsConditions = false;
        }

        // check if player has the additional structure (if required)
        if (upgrade.needsStructureType > -1) {
            bool hasAtleastOneNeedStructureType = player->hasAtleastOneStructure(upgrade.needsStructureType);
            if (!hasAtleastOneNeedStructureType) {
                meetsConditions = false;
            }
        }

        // check if the structure to upgrade is at the expected level
        int structureUpgradeLevel = player->getStructureUpgradeLevel(upgrade.structureType);

        if (structureUpgradeLevel != upgrade.atUpgradeLevel) {
            meetsConditions = false;
        }

        if (meetsConditions) {
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
                        player->credits += item->getRefundAmount();
                    }
                    item->setIsBuilding(false);
                    item->setProgress(0);
                    cItemBuilder *itemBuilder = player->getItemBuilder();
                    itemBuilder->removeItemFromList(item);
                }
            }

            // remove it
            listUpgrades->removeItemFromListByBuildId(i);
        }
    }

    cLogger::getInstance()->logCommentLine("evaluateUpgrades - end");
}

/**
 * method called, when buildingListItem (the upgrade) has finished building.
 */
void cBuildingListUpdater::onUpgradeCompleted(cBuildingListItem *item) {
	assert(item);
	cLogger::getInstance()->logCommentLine("updateUpgradeCompleted - begin");

    // activate/deactivate any lists if needed
    cSideBar *sideBar = player->getSideBar();

    s_Upgrade upgradeType = upgrades[item->getBuildId()];

    player->increaseStructureUpgradeLevel(upgradeType.structureType);

    assert(upgradeType.providesTypeId > -1);
    assert(upgradeType.providesType > -1);

    assert(upgradeType.providesTypeList > -1);
    assert(upgradeType.providesTypeList < LIST_UPGRADES);

    cBuildingList *list = sideBar->getList(upgradeType.providesTypeList);
    if (upgradeType.providesType == UNIT) {
        list->addUnitToList(upgradeType.providesTypeId, upgradeType.providesTypeSubList);
    } else if (upgradeType.providesType == STRUCTURE) {
        list->addStructureToList(upgradeType.providesTypeId, upgradeType.providesTypeSubList);
    }

    evaluateUpgrades();

	cLogger::getInstance()->logCommentLine("updateUpgradeCompleted - end");
}
