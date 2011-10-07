#ifndef CBUILDINGLISTUPDATER_H_
#define CBUILDINGLISTUPDATER_H_

#include "../sidebar/cBuildingList.h"
#include "../sidebar/cSideBar.h"

// FIXME: / TODO: CYCLIC DEPENDENCY WITH PLAYER CLASS!!

class cBuildingListUpdater {
	public:
		cBuildingListUpdater(cPlayer *thePlayer);

		// update methods are event based. Ie, when structure is created,
		// when structure is destroyed, etc

		// structure created and placed (ie, building windtrap, gives refinery in list, etc)
		void updateStructureCreated(int structureType);

		// structure destroyed..
		void updateStructureDestroyed(int structureType);

		// upgrade completed, like add 4slab
		void updateUpgradeCompleted(cBuildingList *listToUpgrade);

	private:
		// this sidebar will be updated
		cSideBar *sidebar;

		// this player will be used to read state from
		// in order to know what to update
		cPlayer *player;
};

#endif /* CBUILDINGLISTUPDATER_H_ */
