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

#include "sidebar/cBuildingListItem.h"

// forward declr..
class cItemBuilder;

class cBuildingList {
public:
    cBuildingList(eListType listType);

    ~cBuildingList();

    cBuildingListItem *getItem(int i);    // return building list item in array at position.
    cBuildingListItem *getFirstItemInSubList(int sublistId);
    cBuildingListItem *getItemToPlace();        // return building list item with placeIt = true.
    cBuildingListItem *getItemToDeploy();       // return building list item with deployIt = true.

    bool removeItemFromList(cBuildingListItem *item);

    bool removeItemFromList(int position);

    bool removeItemFromListByBuildId(int type);

    void addUpgradeToList(int structureTypeToUpgrade);

    void addStructureToList(int structureType, int subList);

    void addSpecialToList(int specialType, int subList);

    void addUnitToList(int unitType, int subList);

    bool addItemToList(cBuildingListItem *item);

    bool isOverButton(int x, int y);

    void removeAllItems();

    bool isItemInList(cBuildingListItem *item);

    // get
    bool isSelected() { return selected; }

    int getLastClickedId() { return lastClickedId; }

    int getButtonIconIdPressed() { return buttonIconIdPressed; }

    int getButtonIconIdUnpressed() { return buttonIconIdUnpressed; }

    int getButtonDrawX() { return buttonDrawX; }

    int getButtonDrawY() { return buttonDrawY; }

    bool isAvailable() { return maxItems > 0; }

    std::array<int, 5> isBuildingItem();

    cBuildingListItem *getItemByBuildId(int buildId);

    // set
    void setSelected(bool value) { selected = value; }

    void setStatusPendingUpgrade(int subListId);

    void setStatusPendingBuilding(int subListId);

    void setStatusAvailable(int subListId);

    void setButtonIconIdPressed(int iconId) { buttonIconIdPressed = iconId; }

    void setButtonIconIdUnpressed(int iconId) { buttonIconIdUnpressed = iconId; }

    void setButtonWidth(int value) { buttonWidth = value; }

    void setButtonHeight(int value) { buttonHeight = value; }

    void setButtonDrawX(int value) { buttonDrawX = value; }

    void setButtonDrawY(int value) { buttonDrawY = value; }

    void setLastClickedId(int value) { lastClickedId = value; }

    /**
    * type of list (see cSideBarFactory) , ie 0 = CONSTYARD, last is IX (sUpgradeInfo), etc.
    * @param value
    */
    eListType getType() { return typeOfList; }

    void removeAllSublistItems(int sublistId);

    void resetTimesOrderedForAllItems();

    void setItemBuilder(cItemBuilder *value);

    void startFlashing();

    bool isFlashing() { return TIMER_flashing > 0; }

    int getFlashingColor();

    void think();

    void stopFlashing();

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

    int TIMER_flashing;

    int maxItems;         // how many items have been set in the list?

    bool selected;          // is this list selected at sidebar or not?

    // the list of icons
    cBuildingListItem *items[MAX_ITEMS];

    cItemBuilder *m_itemBuilder;

    int getFreeSlot();

    eListType typeOfList;
};

#endif
