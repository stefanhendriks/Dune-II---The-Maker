#pragma once

#include "cSideBar.h"
#include "definitions.h"

#include "sidebar/cBuildingListItem.h"

#include <array>
#include "utils/Color.hpp"

class cItemBuilder;

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
class cBuildingList {
public:
    explicit cBuildingList(eListType listType);

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
    bool isSelected() {
        return m_selected;
    }

    int getLastClickedId() {
        return m_lastClickedId;
    }

    int getButtonIconIdPressed() {
        return m_buttonIconIdPressed;
    }

    int getButtonIconIdUnpressed() {
        return m_buttonIconIdUnpressed;
    }

    int getButtonDrawX() {
        return m_buttonDrawX;
    }

    int getButtonDrawY() {
        return m_buttonDrawY;
    }

    bool isAvailable() {
        return m_maxItems > 0;
    }

    std::array<int, 6> isBuildingItem();

    cBuildingListItem *getItemByBuildId(int buildId);

    // set
    void setSelected(bool value) {
        m_selected = value;
    }

    void setStatusPendingUpgrade(int subListId);

    void setStatusPendingBuilding(int subListId);

    void setStatusAvailable(int subListId);

    void setButtonIconIdPressed(int iconId) {
        m_buttonIconIdPressed = iconId;
    }

    void setButtonIconIdUnpressed(int iconId) {
        m_buttonIconIdUnpressed = iconId;
    }

    void setButtonWidth(int value) {
        m_buttonWidth = value;
    }

    void setButtonHeight(int value) {
        m_buttonHeight = value;
    }

    void setButtonDrawX(int value) {
        m_buttonDrawX = value;
    }

    void setButtonDrawY(int value) {
        m_buttonDrawY = value;
    }

    void setLastClickedId(int value) {
        m_lastClickedId = value;
    }

    /**
    * type of list (see cSideBarFactory) , ie 0 = CONSTYARD, last is IX (sUpgradeInfo), etc.
    * @param value
    */
    eListType getType() {
        return m_typeOfList;
    }

    void removeAllSublistItems(int sublistId);

    void clearList();

    void resetTimesOrderedForAllItems();

    void setItemBuilder(cItemBuilder *value);

    void startFlashing();

    bool isFlashing() {
        return m_TIMER_flashing > 0;
    }

    Color getFlashingColor();

    void think();

    void stopFlashing();

protected:
    // nothing

private:
    int m_lastClickedId;    // last clicked item id
    int m_buttonIconIdPressed;    // the button to draw at the left of the list when pressed (selected)
    int m_buttonIconIdUnpressed;    // the button to draw at the left of the list when unpressed
    int m_buttonDrawX;
    int m_buttonDrawY;
    int m_buttonWidth;
    int m_buttonHeight;

    int m_TIMER_flashing;

    int m_maxItems;         // how many items have been set in the list?

    bool m_selected;          // is this list selected at sidebar or not?

    // the list of icons
    cBuildingListItem *m_items[MAX_ITEMS];

    cItemBuilder *m_itemBuilder;

    int getFreeSlot();

    eListType m_typeOfList;
};
