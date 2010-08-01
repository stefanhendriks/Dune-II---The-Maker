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

#ifndef CBUILDINGLIST
#define CBUILDINGLIST

class cBuildingList {
	public:
		cBuildingList(int type);
		~cBuildingList();

		cBuildingListItem * getItem(int position); 	// return building list item in array at position.
		cBuildingListItem * getItemToPlace();		// return building list item with placeIt = true.

		// add item to the list
		void addItemToList(cBuildingListItem * item);
		void removeItemFromList(int position);

		bool isOverButton(int x, int y);
		void removeAllItems();

		// get
		bool isUpgrading() { return upgrading; }
		int getTimer() { return TIMER_progress; }
		int getLastClickedId() { return lastClickedId; }
		int getButtonIconId() { return buttonIconId; }
		int getButtonDrawX() { return buttonDrawX; }
		int getButtonDrawY() { return buttonDrawY; }
		int getScrollingOffset() { return scrollingOffset; }
		bool isAvailable() { return available; }
		bool isBuildingItem();

		void scrollUp();
		void scrollDown();

		// set
		void setTimer(int value) { TIMER_progress = value; }


		/**
		 * Remember, only the pressed version. Unpressed icon versions are not available. They are drawn
		 * within the HERALD_XXX overall GUI picture.
		 * @param iconId
		 */
		void setButtonIcon(int iconId) { buttonIconId = iconId; }


		void setButtonDrawX(int value) { buttonDrawX = value; }
		void setButtonDrawY(int value) { buttonDrawY = value; }
		void setScrollingOffset(int value);
		void setAvailable(bool value) { available = value; }
		void setLastClickedId(int value) { lastClickedId = value; }

		int getType() { return typeOfList; }

	protected:
		// nothing

	private:
		int TIMER_progress;	// timer for upping progress
		bool upgrading;		// upgrading this field of technology?
		int lastClickedId;	// last clicked item id
		int buttonIconId;	// the button to draw at the left of the list
		int buttonDrawX;
		int buttonDrawY;
		int scrollingOffset; // the offset for scrolling through the list.
		bool available;		 // is this list available?
		int maxItems;		 // how many items have been set in the list?

		// the list of icons
		cBuildingListItem * items[MAX_ITEMS];

		int getFreeSlot();
		int typeOfList;
};

#endif
