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

class cPlayer {

	public:
		cPlayer();
		~cPlayer();

		PALETTE pal;		// each player has its own 256 color scheme (used for coloring units)

		void init(int id);

		int iTeam;

		int use_power;
		int has_power;

		float credits;		// the credits this player has
		float max_credits;	// max credits a player can have (influenced by silo's)

		int focus_cell;		// this is the cell that will be showed in the game centralized upon map loading

		int iPrimaryBuilding[MAX_STRUCTURETYPES];	// remember the primary ID (structure id) of each structure type
		int iStructures[MAX_STRUCTURETYPES]; // remember what is built for each type of structure

		bool bEnoughPower() const;

		int TIMER_think;        // timer for thinking itself (calling main routine)
		int TIMER_attack;       // -1 = determine if its ok to attack, > 0 is , decrease timer, 0 = attack

		void substractCredits(int amount) {
            credits -= amount;
        };

		// set
		void setItemBuilder(cItemBuilder *theItemBuilder);
		void setSideBar(cSideBar *theSideBar);
		void setBuildingListUpdater(cBuildingListUpdater *theBuildingListUpgrader);
		void setTechLevel(int theTechLevel) { techLevel = theTechLevel; }
		void setHouse(int iHouse);
		void setStructurePlacer(cStructurePlacer *theStructurePlacer);
		void setUpgradeBuilder(cUpgradeBuilder *theUpgradeBuilder);
		void setOrderProcesser(cOrderProcesser *theOrderProcesser);
		void setGameControlsContext(cGameControlsContext *theGameControlsContext);

		// get
		cBuildingListUpdater *getBuildingListUpdater() { return buildingListUpdater; }
		cPlayerDifficultySettings *getDifficultySettings() const { return difficultySettings; }
		cItemBuilder *getItemBuilder() const { return itemBuilder; }
		cSideBar *getSideBar() const { return sidebar; }
		int getHouse() const { return house; }
		bool isHouse(int houseId ) const { return house == houseId; }
		int getTechLevel() const { return techLevel; }
		int getId() const { return id; }
		cStructurePlacer * getStructurePlacer() const { return structurePlacer; }
		cUpgradeBuilder * getUpgradeBuilder() const { return upgradeBuilder; }
		cOrderProcesser * getOrderProcesser() const { return orderProcesser; }
		cGameControlsContext * getGameControlsContext() const { return gameControlsContext; }
		int getMinimapColor() const { return minimapColor; }
        int getEmblemBackgroundColor() const { return emblemBackgroundColor; }
		bool isHuman() {
		    return m_Human;
		}

		int getAmountOfStructuresForType(int structureType) const;
		bool hasRadarAndEnoughPower() const;

		// delete
		void deleteSideBar() { if (sidebar) delete sidebar; }


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

    bool hasAtleastOneStructure(int structureType);

    bool hasEnoughCreditsFor(float requestedAmount);

private:
		int getRGBColorForHouse(int houseId);
		int getEmblemBackgroundColorForHouse(int houseId);
		bool m_Human;

		// TODO: in the end this should be redundant.. perhaps remove it now/soon anyway?
		// TODO: redundant? OBSELETE. Since we're getting more properties for units and thereby
		// can/should create units specific for houses.
		cPlayerDifficultySettings *difficultySettings;

		// these have all state, and need to be recreated for each mission.
		cSideBar * sidebar;			// each player has a sidebar (lists of what it can build)
		cItemBuilder * itemBuilder; // each player can build items
		cUpgradeBuilder * upgradeBuilder; // each player can upgrade lists

		cBuildingListUpdater * buildingListUpdater; // modifies list of sidebar on upgrades
		cStructurePlacer * structurePlacer;	// used to place structures and handle updates in sidebar accordingly
		cOrderProcesser * orderProcesser; // process orders for starport

		cGameControlsContext * gameControlsContext;

		int techLevel;		// technology level
		int house;
		int minimapColor;			// color of this team on minimap;
		int emblemBackgroundColor;	// color of the emblem background
		int id;	// this id is the reference to the player array

		BITMAP * bmp_structure[MAX_STRUCTURETYPES];

		BITMAP * bmp_unit[MAX_UNITTYPES]; // the body of unit
		BITMAP * bmp_unit_top[MAX_UNITTYPES]; // optionally a 'top' of unit (barrel of tank, for example)

        void clearStructureTypeBitmaps();
        void clearUnitTypeBitmaps();

};

#endif
