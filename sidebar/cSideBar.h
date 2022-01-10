/*
 * cSideBar.h
 *
 * The sidebar consists out of several parts:
 * - buildLists (the icons that represent them, and a list of icons of the
 *   selected list)
 * - a minimap
 * - capacity indicators
 *
 *  Created on: Aug 1, 2009
 *      Author: Stefan
 */

#ifndef CSIDEBAR_H_
#define CSIDEBAR_H_

class cPlayer;

//// List ID's corresponding buttons
//#define LIST_NONE        0
//#define LIST_CONSTYARD    1
//#define LIST_FOOT_UNITS    2 // infantry, soldier, trooper, troopers, etc.
//#define LIST_UNITS        3 // quad, trike, tanks, air units
//#define LIST_STARPORT    4 // special list to order stuff
//#define LIST_PALACE        5 // REMOVE? (ie super weapon icon should always show top left/right?)
//#define LIST_UPGRADES    6 // possible upgrades
//#define LIST_MAX        7 // max amount of lists

#define SUBLIST_CONSTYARD  0  // structures
#define SUBLIST_LIGHTFCTRY 1 // quad, trike, etc
#define SUBLIST_HEAVYFCTRY 2 // tanks
#define SUBLIST_HIGHTECH   3 // orni, carry-all

#define SUBLIST_INFANTRY   4 // soldier, infantry
#define SUBLIST_TROOPERS   5 // trooper, troopers

/**
 * Basically a container:
 * - building lists
 * - minimap
 * - capacities (max spice, max power)
 *
 * NOTE: Use the cSideBarFactory to construct a cSideBar class.
 *
 */
class cSideBar {

public:
    cSideBar(cPlayer *thePlayer);

    ~cSideBar();

    cBuildingList *getList(int listId) { return lists[listId]; }

    cBuildingList *getList(eListType listType) { return getList(eListTypeAsInt(listType)); }

    bool startBuildingItemIfOk(eListType listType, int buildId) const;

    void setList(eListType listType, cBuildingList *list);

    int getSelectedListID() { return selectedListID; }
    cBuildingListItem * getBuildingListItem(eListType listType, int buildId) const;

    void think();    // timer based

    // set
    // no set Player, re-create Sidebar object instead if needed
    void setSelectedListId(eListType value);

    static const int LIST_MAX = 7;

    static const int VerticalCandyBarWidth = 24;
    static const int SidebarWidthWithoutCandyBar = 200;
    static const int SidebarWidth = SidebarWidthWithoutCandyBar + VerticalCandyBarWidth; // including vertical candy bar
    static const int TopBarHeight = 42;
    static const int HeightOfMinimap = 200 + 8;
    static const int WidthOfMinimap = 200;
    static const int PowerBarMarginHeight = 32; // for a power icon

    // Horizontal candybar is below minimap
    static const int HorizontalCandyBarHeight = 12;

    static const int HeightOfListButton = 34;

    static const int TotalHeightBeforePowerBarStarts =
            TopBarHeight + HeightOfMinimap + HorizontalCandyBarHeight + PowerBarMarginHeight;

    void onNotifyMouseEvent(const s_MouseEvent &event);

    void cancelBuildingListItem(cBuildingListItem *item);

    void onNotify(const s_GameEvent &event);

    void findFirstActiveListAndSelectIt();

private:
    void onMouseAt(const s_MouseEvent &event);
    void onMouseClickedLeft(const s_MouseEvent &event);
    void onMouseClickedRight(const s_MouseEvent &event);

    // the lists:
    cBuildingList *lists[LIST_MAX];
    cPlayer *player;

    bool isMouseOverSidebarValue;

    int selectedListID;

    bool startBuildingItemIfOk(cBuildingListItem *item) const;

    void drawMessageBarWithItemInfo(cBuildingListItem *item) const;

    void thinkProgressAnimation();

    cBuildingList *getSelectedList() const;

    void onListBecameUnavailableEvent(const s_GameEvent &event);

    void onListBecameAvailableEvent(const s_GameEvent &event);

    void onSpecialReadyToDeployEvent(const s_GameEvent &event) const;

    void onListItemReadyToPlaceEvent(const s_GameEvent &event) const;
};

#endif /* CSIDEBAR_H_ */
