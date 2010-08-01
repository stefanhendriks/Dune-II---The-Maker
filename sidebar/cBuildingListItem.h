/**
 * a cListItem is  used within a BuildingList , it has a picture and any unit or
 * structure ID attached. At the same time this item is responsible for 'building'
 * the item it represents. It has therefor also other properties; progress a timer, etc.
 *
 *
 */
#ifndef CBUILDINGLISTITEM
#define CBUILDINGLISTITEM

class cBuildingListItem {

public:
	// constructors
	cBuildingListItem(int theID, s_Structures entry, cBuildingList* list);
	cBuildingListItem(int theID, s_UnitP entry, cBuildingList* list);


	// gettters
	int getIconId() { return icon; }
	int getBuildId() { return ID; }
	eBuildType getBuildType() { return type; }
	int getBuildCost() { return cost; }
	int getProgress() { return progress; }
	bool isBuilding() { return building; }
	bool isAvailable() { return available; }
	int getTimesToBuild() { return timesToBuild; }
	int getSlotId() { return slotId; }

	bool canPay();

	float getCreditsPerProgressTime() { return creditsPerProgressTime; }

	float getRefundAmount();

	bool shouldPlaceIt() { return placeIt; }

	// setters
	void setIconId(int value) { icon = value; }
	void setBuildId(int value) { ID = value; }
	void setBuildType(eBuildType value) { type = value; }
	void setBuildCost(int value) { cost = value; }
	void setProgress(int value) { progress = value; }
	void setIsBuilding(bool value) { building = value; }
	void setIsAvailable(bool value) { available = value; }
	void setTimesToBuild(int value) { timesToBuild = value; }
	void increaseTimesToBuild() { timesToBuild++; }
	void decreaseTimesToBuild();
	void setSlotId(int value) { slotId = value; }
	void setPlaceIt(bool value) { placeIt = value; }


	cBuildingList *getList() { return myList; }	// returns the list it belongs to

private:
	int icon;				// the icon ID to draw (from datafile)
	int ID;					// the ID to build ..
	eBuildType type;		// .. of this type of thing (ie, UNIT or STRUCTURE)
	int cost;				// price
	bool building;			// building this item? (default = false)
	bool available;			// available, is set to false when one item is building , the others turn not available
							// note; this is not the same as not being able to build it.
	int progress;			// progress building this item
	int timesToBuild;		// the amount of times to build this item (queueing) (meaning, when building = true, this should be 1...)
	int slotId;			 	// return index of items[] array (set after adding item to list, default is < 0)

	float creditsPerProgressTime; // credits to pay for each progress point. (calculated at creation)
	bool placeIt;			// when true, this item is ready for placement.

	cBuildingList *myList;
};

#endif
