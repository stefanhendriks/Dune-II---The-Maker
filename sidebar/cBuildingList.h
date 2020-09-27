/**
 *
 * This is a class, representing a list of icons that can be selected and built.
 *
 * Per list one item can be built at a time. Each list has therefore a timer.
 *
 * A list has also a corresponding icon the user can click on, in order to view the
 * lists contents. (these are the icons left to the building list). Each list has the
 * ID of the unpressed icon picture, and also the coordinates for this icon.
 *
 */

#include <array>

#ifndef CBUILDINGLIST
#define CBUILDINGLIST

class cBuildingList {
	public:
		cBuildingList(int theId);
		~cBuildingList();

		cBuildingListItem * getItem(int position); 	// return building list item in array at position.
		cBuildingListItem * getItemToPlace();		// return building list item with placeIt = true.

		void removeItemFromList(int position);

        void addStructureToList(int structureType, int subList);
        void addUnitToList(int unitType, int subList);

        // generic way, ie by upgradeBuilder
        bool addItemToList(cBuildingListItem * item);

		bool isOverButton(int x, int y);
		void removeAllItems();
		bool isItemInList(cBuildingListItem * item);

		// get
		bool isUpgrading() { return upgrading; }
		int getTimer() { return TIMER_progress; }
		int getLastClickedId() { return lastClickedId; }
		int getButtonIconIdPressed() { return buttonIconIdPressed; }
		int getButtonIconIdUnpressed() { return buttonIconIdUnpressed; }
		int getButtonDrawX() { return buttonDrawX; }
		int getButtonDrawY() { return buttonDrawY; }
		bool isAvailable() { return available; }
		bool isAcceptsOrders() { return acceptsOrders; }
        std::array<int, 5> isBuildingItem();
		bool hasItemType(int itemTypeId);

		// set
		void setTimer(int value) { TIMER_progress = value; }
		void setTypeOfList(int value) { typeOfList = value; }
		void setUpgradeLevel(int value) { upgradeLevel = value; }
		void setUpgrading(bool value) { upgrading = value; }

		void setButtonIconIdPressed(int iconId) { buttonIconIdPressed = iconId; }
		void setButtonIconIdUnpressed(int iconId) { buttonIconIdUnpressed = iconId; }

		void setButtonDrawX(int value) { buttonDrawX = value; }
		void setButtonDrawY(int value) { buttonDrawY = value; }
		void setAvailable(bool value) { available = value; }
		void setLastClickedId(int value) { lastClickedId = value; }
		void setAcceptsOrders(bool value) { acceptsOrders = value; }

		int getType() { return typeOfList; }
		int getUpgradeLevel() { return upgradeLevel; }

    void increaseUpgradeLevel();

protected:
		// nothing

	private:
		int TIMER_progress;	// timer for upping progress
		bool upgrading;		// upgrading this field of technology?
		int lastClickedId;	// last clicked item id
		int buttonIconIdPressed;	// the button to draw at the left of the list when pressed (selected)
		int buttonIconIdUnpressed;	// the button to draw at the left of the list when unpressed
		int buttonDrawX;
		int buttonDrawY;
		bool available;		 // is this list available?
		bool acceptsOrders;	 // accepts orders
		int maxItems;		 // how many items have been set in the list?

		// the list of icons
		cBuildingListItem * items[MAX_ITEMS];

		int getFreeSlot();
		int typeOfList;
		int upgradeLevel;

};

#endif
