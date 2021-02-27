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
#include "cSideBar.h"

#ifndef CBUILDINGLIST
#define CBUILDINGLIST

// forward declr..
class cItemBuilder;

class cBuildingList {
public:
    cBuildingList(eListType listType);

    ~cBuildingList();

    cBuildingListItem *getItem(int i);    // return building list item in array at position.
    cBuildingListItem *getFirstItemInSubList(int sublistId);
    cBuildingListItem *getItemToPlace();        // return building list item with placeIt = true.

    bool removeItemFromList(cBuildingListItem *item);

    bool removeItemFromList(int position);

    bool removeItemFromListByBuildId(int type);

    void addUpgradeToList(int structureTypeToUpgrade);

    void addStructureToList(int structureType, int subList);

    void addUnitToList(int unitType, int subList);

    bool addItemToList(cBuildingListItem *item);

    bool isOverButton(int x, int y);

    void removeAllItems();

    bool isItemInList(cBuildingListItem *item);

    // get
    int getTimer() { return TIMER_progress; }

    int getLastClickedId() { return lastClickedId; }

    int getButtonIconIdPressed() { return buttonIconIdPressed; }

    int getButtonIconIdUnpressed() { return buttonIconIdUnpressed; }

    int getButtonDrawX() { return buttonDrawX; }

    int getButtonDrawY() { return buttonDrawY; }

    bool isAvailable() { return available; }

    bool isAcceptsOrders() { return acceptsOrders; }

    std::array<int, 5> isBuildingItem();

    cBuildingListItem *getItemByBuildId(int buildId);

    // set
    void setTimer(int value) { TIMER_progress = value; }

    void setStatusPendingUpgrade(int subListId);

    void setStatusPendingBuilding(int subListId);

    void setStatusAvailable(int subListId);

    void setButtonIconIdPressed(int iconId) { buttonIconIdPressed = iconId; }

    void setButtonIconIdUnpressed(int iconId) { buttonIconIdUnpressed = iconId; }

    void setButtonWidth(int value) { buttonWidth = value; }

    void setButtonHeight(int value) { buttonHeight = value; }

    void setButtonDrawX(int value) { buttonDrawX = value; }

    void setButtonDrawY(int value) { buttonDrawY = value; }

    void setAvailable(bool value) { available = value; }

    void setLastClickedId(int value) { lastClickedId = value; }

    void setAcceptsOrders(bool value) { acceptsOrders = value; }

    /**
    * type of list (see cSideBarFactory) , ie 0 = CONSTYARD, last is IX (upgrades), etc.
    * @param value
    */
    eListType getType() { return typeOfList; }

    void removeAllSublistItems(int sublistId);

    void resetTimesOrderedForAllItems();

    void setItemBuilder(cItemBuilder *value) { m_itemBuilder = value; }

protected:
    // nothing

private:
    int TIMER_progress;    // timer for upping progress

    int lastClickedId;    // last clicked item id
    int buttonIconIdPressed;    // the button to draw at the left of the list when pressed (selected)
    int buttonIconIdUnpressed;    // the button to draw at the left of the list when unpressed
    int buttonDrawX;
    int buttonDrawY;
    int buttonWidth;
    int buttonHeight;

    bool available;         // is this list available?
    bool acceptsOrders;     // accepts orders
    int maxItems;         // how many items have been set in the list?

    // the list of icons
    cBuildingListItem *items[MAX_ITEMS];

    cItemBuilder *m_itemBuilder;

    int getFreeSlot();

    eListType typeOfList;
};

#endif
