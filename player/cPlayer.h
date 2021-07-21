/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
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

#include <vector>

class cPlayer : public cScenarioObserver {

public:
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

    eHouseBitFlag getHouseBitFlag();

    int getHouseFadingColor() const;

    int getErrorFadingColor() const;

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

    std::string asString() const {
        char msg[512];
        sprintf(msg, "Player [id=%d, human=%b, sidebar=%d]", this->id, this->m_Human, this->sidebar);
        return std::string(msg);
    }

    BITMAP *getStructureBitmap(int index);

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


    std::vector<int> getAllMyUnits();

    std::vector<int> getAllMyStructuresAsId();

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
     * Check if we can build unit of type. Returns a reason when it can't. If the reason is NONE it means it can be built.
     */
    eCantBuildReason canBuildUnit(int iUnitType);


    /**
     * Checks if the given structureType is available for producing; does not check if it is allowed to do so. Use
     * canBuildStructure for that.
     *
     * @param iStructureType
     * @return
     */
    bool canBuildStructureType(int iStructureType) const;
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
    cAbstractStructure* placeStructure(int destinationCell, cBuildingListItem *itemToPlace);

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

    static std::string getHouseNameForId(int house);

    void logStructures();

    void cancelBuildingListItem(cBuildingListItem *item);

    void cancelStructureBuildingListItemBeingBuilt();

private:
    cBuildingListItem *isUpgradeAvailableToGrant(eBuildType providesType, int providesTypeId) const;

    int getRGBColorForHouse(int houseId);

    int getEmblemBackgroundColorForHouse(int houseId);

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


    BITMAP *bmp_structure[MAX_STRUCTURETYPES];

    BITMAP *bmp_unit[MAX_UNITTYPES]; // the body of unit
    BITMAP *bmp_unit_top[MAX_UNITTYPES]; // optionally a 'top' of unit (barrel of tank, for example)

    void clearStructureTypeBitmaps();

    void clearUnitTypeBitmaps();

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

};

#endif
