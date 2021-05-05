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

class cPlayer {

public:
    cPlayer();

    ~cPlayer();

    PALETTE pal;        // each player has its own 256 color scheme (used for coloring units)

    void init(int id, cPlayerBrain *brain);

    void setBrain(cPlayerBrain *brain);

    bool bEnoughPower() const;

    bool bEnoughSpiceCapacityToStoreCredits() const;

    // credits
    void substractCredits(int amount);
    void setCredits(int credits);
    void giveCredits(float amountToGive);
    int getCredits();


    // focus cell
    void setFocusCell(int cll);
    int getFocusCell() { return focusCell_; }

    // house
    std::string getHouseName();
    int getHouse() const { return house; }
    bool isHouse(int houseId) const { return house == houseId; }
    void setHouse(int iHouse);

    // ---
    void setItemBuilder(cItemBuilder *theItemBuilder);

    void setSideBar(cSideBar *theSideBar);

    void setBuildingListUpdater(cBuildingListUpdater *theBuildingListUpgrader);

    void setTechLevel(int theTechLevel) { techLevel = theTechLevel; }

    void setStructurePlacer(cStructurePlacer *theStructurePlacer);

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

    cStructurePlacer *getStructurePlacer() const { return structurePlacer; }

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

    std::vector<int> getAllMyStructures();

    bool isSameTeamAs(cPlayer *pPlayer);

    void update();

    float getMaxCredits();

    int getPowerProduced();

    int getPowerUsage();

    void dumpCredits(int amount);

    void think();

    bool canBuildUnitType(int iUnitType) const;
    bool canBuildStructureType(int iStructureType) const;

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
    void startBuildingStructure(int iStructureType) const;
    void startUpgrading(int iUpgradeType) const;

    eCantBuildReason canBuildUnit(int iUnitType);

    int findRandomUnitTarget(int playerIndexToAttack);
    int findRandomStructureTarget(int iAttackPlayer);

private:
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
    cStructurePlacer *structurePlacer;    // used to place structures and handle updates in sidebar accordingly
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
    std::string getHouseNameForId(int house) const;

    int powerUsage_;      // total amount of power usage
    int powerProduce_;      // total amount of power generated

    float credits;        // the credits this player has
    float maxCredits_;    // max credits a player can have (influenced by silo's)

    int focusCell_;        // this is the cell that will be showed in the game centralized upon map loading

    cPlayerBrain *brain_;
};

#endif
