#ifndef CBUILDINGLISTUPDATER_H_
#define CBUILDINGLISTUPDATER_H_

class cPlayer;

class cBuildingListUpdater {
	public:
		cBuildingListUpdater(cPlayer * thePlayer);

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
		cPlayer * player;

        void applyUpgrade(const s_UpgradeInfo &upgradeType);

        void onStructureCreatedSkirmishMode(int structureType) const;

        void onStructureCreatedCampaignMode(int structureType) const;

        void onStructureDestroyedSkirmishMode(int structureType) const;

        void onStructureDestroyedCampaignMode(int structureType) const;

};

#endif /* CBUILDINGLISTUPDATER_H_ */
