/**
 * @file cBuildingListUpdater.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include "cBuildingListItem.h"

class cPlayer;
class cInfoContext;
class cGameSettings;
struct sGameServices;

class cBuildingListUpdater {
public:
    explicit cBuildingListUpdater(cPlayer *thePlayer);
    void serviceInit(sGameServices* services);

    // update methods are event based. Ie, when structure is created,
    // when structure is destroyed, etc

    // structure created and placed (ie, building windtrap, gives refinery in list, etc)
    void onStructureCreated(int structureType);

    // structure destroyed..
    void onStructureDestroyed(int structureType);

    // upgrade completed, like add 4slab
    void onUpgradeCompleted(cBuildingListItem *item);

    void evaluateUpgrades();

    void onUpgradeStarted(cBuildingListItem *pItem);

    void onUpgradeCancelled(cBuildingListItem *pItem);

    void onBuildItemCancelled(cBuildingListItem *pItem);

    void onBuildItemStarted(cBuildingListItem *pItem);

    void onBuildItemCompleted(cBuildingListItem *pItem);

private:
    // this player will be used to read state from
    // in order to know what to update
    cPlayer *m_player;
    cInfoContext* m_info = nullptr;
    cGameSettings* m_settings = nullptr;

    void applyUpgrade(const s_UpgradeInfo &upgradeType);

    void onStructureCreatedSkirmishMode(int structureType) const;

    void onStructureCreatedCampaignMode(int structureType) const;

    void onStructureDestroyedSkirmishMode() const;

    void onStructureDestroyedCampaignMode() const;

};
