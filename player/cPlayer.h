/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2010 (c) code by Stefan Hendriks

  A player has a sidebar. The sidebar contains lists of items that can be built. These
  items can be structures/units but also special weapons, etc.

  In order to build items, a player has an itemBuilder.

  A player can upgrade the sidebar lists. Therefore a cBuildingListUpgrader is used.
  The state of upgrades is held by a cPlayerUpgradeState object.

  */
#ifndef PLAYER_H
#define PLAYER_H

#include "cPlayerNotification.h"

#include <set>
#include <string>
#include <vector>

class cAbstractStructure;
class cItemBuilder;
class cOrderProcesser;
class cSideBar;
class cPlayerDifficultySettings;

struct s_PlaceResult {
    bool success = false; // if true, all is ok

    bool onlyMyUnitsBlock = false;  // if true, then it means when success=false,
    // ONLY units block the placement, but they are the units of the requesting player.
    // which can be fixed (by AI) to move them away.

    // else, these are one of the reasons why it was not a success
    bool badTerrain = false; // not valid to place

    bool outOfBounds = false; // (one of the cells) is out of bounds

    // these units block it
    std::set<int> unitIds = std::set<int>();

    // these structures block it
    std::set<int> structureIds = std::set<int>();
};

class cPlayer : public cScenarioObserver {

public:
    static const int MAX_STRUCTURE_BMPS = MAX_STRUCTURETYPES*2;
    cPlayer();

    ~cPlayer();

    PALETTE pal;        // each player has its own 256 color scheme (used for coloring units)

    void init(int id, brains::cPlayerBrain *brain);

    void setBrain(brains::cPlayerBrain *brain);

    void setAutoSlabStructures(bool value);

    bool bEnoughPower() const;

    bool bEnoughSpiceCapacityToStoreCredits() const;

    bool hasAlmostReachMaxSpiceStorageCapacity() const;

    bool bEnoughSpiceCapacityToStoreCredits(int threshold) const;

    // credits
    void substractCredits(int amount);
    void setCredits(int credits);
    void giveCredits(float amountToGive);
    int getCredits();


    // focus cell
    void setFocusCell(int cll);
    int getFocusCell() { return focusCell_; }

    // house
    std::string getHouseName() const;
    int getHouse() const { return house; }
    bool isHouse(int houseId) const { return house == houseId; }
    void setHouse(int iHouse);

    // ---
    void setItemBuilder(cItemBuilder *theItemBuilder);

    void setSideBar(cSideBar *theSideBar);

    void setBuildingListUpdater(cBuildingListUpdater *theBuildingListUpgrader);

    void setTechLevel(int theTechLevel) { techLevel = theTechLevel; }

    void setOrderProcesser(cOrderProcesser *theOrderProcesser);

    void setGameControlsContext(cGameControlsContext *theGameControlsContext);

    // get
    cBuildingListUpdater *getBuildingListUpdater() const { return buildingListUpdater; }

    cPlayerDifficultySettings *getDifficultySettings() const { return difficultySettings; }

    cItemBuilder *getItemBuilder() const { return itemBuilder; }

    cSideBar *getSideBar() const { return sidebar; }

    cAbstractStructure *getSelectedStructure();

    eHouseBitFlag getHouseBitFlag();

    int getHouseFadingColor() const;

    int getErrorFadingColor() const;

    int getPrimaryBuildingFadingColor() const;

    int getSelectFadingColor() const;

    int getTechLevel() const { return techLevel; }

    int getId() const { return id; }

    cOrderProcesser *getOrderProcesser() const { return orderProcesser; }

    cGameControlsContext *getGameControlsContext() const { return gameControlsContext; }

    int getMinimapColor() const { return minimapColor; }

    int getEmblemBackgroundColor() const { return emblemBackgroundColor; }

    bool isHuman() {
        return m_Human;
    }

    int getAmountOfUnitsForType(int unitType) const;

    int getAmountOfStructuresForType(int structureType) const;

    int getAmountOfUnitsForType(std::vector<int> unitTypes) const;

    bool hasRadarAndEnoughPower() const;

    std::string asString() const;

    BITMAP *getStructureBitmap(int index);

    BITMAP *getStructureBitmapFlash(int index);

    BITMAP *getFlagBitmap();

    BITMAP *getFlagSmallBitmap();

    BITMAP *getUnitBitmap(int index);

    BITMAP *getUnitTopBitmap(int index);

    BITMAP *getUnitShadowBitmap(int index, int bodyFacing, int animationFrame);

    void destroyAllegroBitmaps();

    bool hasWor() const;

    bool hasBarracks() const;

    bool hasAtleastOneStructure(int structureType) const;

    void decreaseStructureAmount(int structureType);

    void increaseStructureAmount(int structureType);

    bool hasEnoughPowerFor(int structureType) const;

    bool hasEnoughCreditsFor(float requestedAmount) const;

    bool hasEnoughCreditsForUnit(int unitType);

    bool hasEnoughCreditsForStructure(int structureType);

    bool hasEnoughCreditsForUpgrade(int upgradeType);

    bool isPrimaryStructureForStructureType(int structureType, int structureId) const {
        return getPrimaryStructureForStructureType(structureType) == structureId;
    }

    int getPrimaryStructureForStructureType(int structureType) const {
        return iPrimaryBuilding[structureType];
    }

    void setPrimaryBuildingForStructureType(int structureType, int structureIndex) {
        iPrimaryBuilding[structureType] = structureIndex;
    }

    int getStructureUpgradeLevel(int structureType) const {
        return iStructureUpgradeLevel[structureType];
    }

    void increaseStructureUpgradeLevel(int structureType) {
        if (structureType < 0) return;
        if (structureType >= MAX_STRUCTURETYPES) return;
        iStructureUpgradeLevel[structureType]++;
    }

    void setTeam(int value) { iTeam = value; }

    int getTeam() { return iTeam; }


    std::vector<int> getAllMyUnits();

    std::vector<int> getAllMyUnitsForType(int unitType) const;

    std::vector<int> getAllMyStructuresAsId();

    std::vector<int> getAllMyStructuresAsIdForType(int structureType);

    bool isSameTeamAs(cPlayer *pPlayer);

    void update();

    float getMaxCredits();

    int getPowerProduced();

    int getPowerUsage();

    void dumpCredits(int amount);

    void think();

    /**
     * Checks if the given unitType is available for producing; does not check if it is allowed to do so. Use
     * canBuildUnit for that.
     *
     * @param iUnitType
     * @return
     */
    bool canBuildUnitType(int iUnitType) const;

    /**
     * Checks if the given iType is available for producing; does not check if it is allowed to do so. Use
     * canBuilSpecial for that.
     *
     * @param iType
     * @return
     */
    bool canBuildSpecialType(int iType) const;

    /**
     * Check if we can build unit of type. Returns a reason when it can't. If the reason is NONE it means it can be built.
     */
    eCantBuildReason canBuildUnit(int iUnitType);

    /**
     * Returns true/false if this house could (ever) build this special type
     * @param iType
     * @return
     */
    bool couldBuildSpecial(int iType);

    eCantBuildReason canBuildSpecial(int iType);

    /**
     * Returns true if the unit can be bought, takes not money into account, but all other rules do apply
     * @param iUnitType
     * @return
     */
    bool canBuildUnitBool(int iUnitType);

    eCantBuildReason canBuildUnit(int iUnitType, bool checkIfAffordable);


    /**
     * Checks if the given structureType is available for producing (in list of constyard);
     * does not check if it is allowed to do so. Use canBuildStructure for that.
     *
     * @param iStructureType
     * @return
     */
    bool isStructureTypeAvailableForConstruction(int iStructureType) const;
    /**
     * Check if we can build structure of type. Returns a reason when it can't. If the reason is NONE it means it can be built.
     */
    eCantBuildReason canBuildStructure(int iStructureType);

    int getStructureTypeBeingBuilt() const;
    cBuildingListItem *getStructureBuildingListItemBeingBuilt() const;

    int  findCellToPlaceStructure(int iStructureType);

    /**
     * Returns true if anything is built from ConstYard
     */
    bool isBuildingStructureAwaitingPlacement() const;
    bool isSpecialAwaitingPlacement() const;
    cBuildingListItem * getSpecialAwaitingPlacement() const;

    cBuildingListItem * isUpgradeAvailableToGrantUnit(int iUnitType) const;
    cBuildingListItem * isUpgradeAvailableToGrantStructure(int iStructureType) const;

    cBuildingListItem * isUpgradingList(int listId, int sublistId) const;
    cBuildingListItem * isUpgradingConstyard() const;
    cBuildingListItem * isBuildingStructure() const;
    bool isBuildingSomethingInSameListSubListAsUnitType(int iUnitType) const;

    bool startBuildingUnit(int iUnitType) const;
    bool startBuildingStructure(int iStructureType) const;
    bool startBuildingSpecial(int iSpecialType) const;
    bool startUpgrading(int iUpgradeType) const;

    bool startUpgradingForUnitIfPossible(int iUpgradeType) const;

    int findRandomUnitTarget(int playerIndexToAttack);
    int findRandomStructureTarget(int iAttackPlayer);

    /**
     * Places structure (if allowed), source is building list item (ie something the player/AI built and wanted to place). Also
     * takes care of the buildingList management after placement. The buildingList management is applied regardless if
     * the structure got placed or not.
     *
     * @param destinationCell   where to put it
     * @param itemToPlace       which building list item to use? (used to derive which kind of structure, etc)
     * @return
     */
    cAbstractStructure* placeItem(int destinationCell, cBuildingListItem *itemToPlace);

    /**
     * Places structure, usually the source is somewhere else (a unit, MCV; or from map loading).
     * @param destinationCell   where to put it
     * @param iStructureTypeId structure type
     * @param healthPercentage 0-100
     * @return
     */
    cAbstractStructure* placeStructure(int destinationCell, int iStructureTypeId, int healthPercentage);

    void onNotify(const s_GameEvent &event);

    int getScoutingUnitType();
    int getSpecialUnitType();

    static std::string getHouseNameForId(int house);

    void logStructures();

    void cancelBuildingListItem(cBuildingListItem *item);

    void cancelStructureBuildingListItemBeingBuilt();

    s_PlaceResult canPlaceStructureAt(int iCell, int iStructureType, int iUnitIDToIgnore);
    s_PlaceResult canPlaceStructureAt(int iCell, int iStructureType);
    s_PlaceResult canPlaceConcreteAt(int iCell, int iStructureType);

    void log(const char *txt) const;

    void thinkFast();

    int getSoldierType();

    int getInfantryType();

    int getSameOrSimilarUnitType(int requestedUnitType);

    void setQuota(int value) { spiceQuota = value; }

    bool hasMetQuota();

    /**
     * Checks if player has anything, if so, it returns true.
     */
    bool evaluateStillAlive();

    bool isAlive() { return alive; }

    std::vector<cPlayerNotification> & getNotifications();

    void addNotification(const char *msg, eNotificationType type);

    bool isNotPlacingSomething();
    bool isNotDeployingSomething();

    // properties (for now public, should become private)
    int selected_structure;

    bool bPlaceIt;		// placing something? (for structures only)
    bool bPlacedIt;		// for remembering, for onCombatMouseEvent stuff..

    bool bDeployIt;		// deploying something? (for palace)
    bool bDeployedIt;   // for remembering, for onCombatMouseEvent stuff..

    void deselectStructure();

private:
    cBuildingListItem *isUpgradeAvailableToGrant(eBuildType providesType, int providesTypeId) const;

    void clearStructureTypeBitmaps();

    void clearUnitTypeBitmaps();

    int getRGBColorForHouse(int houseId);

    int getEmblemBackgroundColorForHouse(int houseId);

    bool isBuildingAnythingForListAndSublist(int listId, int subListId) const;

    bool startBuilding(cBuildingListItem *pItem);

    bool startBuilding(eBuildType buildType, int buildId);

    void onEntityDiscovered(const s_GameEvent &event);

    void onMyUnitDestroyed(const s_GameEvent &event);

    bool m_Human;

    // TODO: in the end this should be redundant.. perhaps remove it now/soon anyway?
    // TODO: redundant? OBSELETE. Since we're getting more properties for units and thereby
    // can/should create units specific for houses.
    cPlayerDifficultySettings *difficultySettings;

    // these have all state, and need to be recreated for each mission.
    cSideBar *sidebar;            // each player has a sidebar (lists of what it can build)
    cItemBuilder *itemBuilder; // each player can build items

    cBuildingListUpdater *buildingListUpdater; // modifies list of sidebar on upgrades
    cOrderProcesser *orderProcesser; // process orders for starport

    cGameControlsContext *gameControlsContext;

    int techLevel;        // technology level
    int house;
    int minimapColor;            // color of this team on minimap;
    int emblemBackgroundColor;    // color of the emblem background
    int id;    // this id is the reference to the player array
    int iTeam;


    BITMAP *bmp_structure[MAX_STRUCTURE_BMPS];
    BITMAP *bmp_flag;
    BITMAP *bmp_flag_small;

    BITMAP *bmp_unit[MAX_UNITTYPES]; // the body of unit
    BITMAP *bmp_unit_top[MAX_UNITTYPES]; // optionally a 'top' of unit (barrel of tank, for example)

    int iPrimaryBuilding[MAX_STRUCTURETYPES];    // remember the primary ID (structure id) of each structure type
    int iStructures[MAX_STRUCTURETYPES]; // remember what is built for each type of structure
    int iStructureUpgradeLevel[MAX_STRUCTURETYPES]; // remember the upgrade level for each structure type

    int powerUsage_;      // total amount of power usage
    int powerProduce_;      // total amount of power generated

    float credits;        // the credits this player has
    float maxCredits_;    // max credits a player can have (influenced by silo's)

    int focusCell_;        // this is the cell that will be showed in the game centralized upon map loading

    brains::cPlayerBrain *brain_;

    bool autoSlabStructures; // flag that will automatically place slabs beneath a structure when placed

    // A condition to win the mission
    int spiceQuota;              // > 0 means this amount to harvest, (if win/lose flags set accordingly for game)

    bool alive;

    std::vector<cPlayerNotification> notifications;

};

#endif
