/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2020 (c) code by Stefan Hendriks


  ---
  New
  ---

  Giving a go for true OOP programming, using a base class and derived classes.
  */
#ifndef CABSTRACTSTRUCTURE_H_
#define CABSTRACTSTRUCTURE_H_

class cAbstractStructure {

	private:
		int armor;			// armor

		int iHitPoints;     // HitPoints this structure has

		int iCell;          // What cell it is (most upper left part of structure where
							// drawing starts)

		int iRallyPoint;    // rallypoint ...

		bool bAnimate;      // Do its special animation? (unit leaving building, starport
							// dropping something, etc)

		bool dead;		    // set to true when die() is called, and thus this structure can be deleted.

		int posX, posY;     // structures do not move, so these are calculated once and then simply returned.


	protected:
        int id;				// the id within the structure[] array

		int iWidth;			// width in cells (set by factory)
		int iHeight;		// height in cells (set by factory)

		int iFrame;         // Frame for animation (flag and other)

		int iPlayer;        // owner

		int iBuildFase;		// Building animation fases

		// Repairing stuff
		bool bRepair;       // repairing? (using timer + gives animation)

    public:

		// Constructor & Destructor:
		cAbstractStructure(); // default constructor

		virtual ~cAbstractStructure();

		float fConcrete;     // how much concrete is *not* beneath this building (percentage)?
							 // meaning, when 0% , it is all concrete. But if 10%, it means 10% of the building
							 // is not covered.

		int iUnitID;        // >-1 means ID to unit


		// TIMERS that all structures use
		int TIMER_repair;   // repairing timer
		int TIMER_flag;     // flag animation
		int TIMER_fade;     // fading timer

		int TIMER_damage;   // damaging stuff
		int TIMER_prebuild; // prebuild timer

		// -------------



		// Filled in by derived classes
		virtual void think()=0;           // think
		virtual void think_animation()=0; // think animation stuff
		virtual void think_guard();       // think guard stuff

		virtual int getType() const = 0;		  // implementation gives type of structure
		void think_prebuild();            // prebuild animation
		void think_repair();              // repair thinking
		void think_damage();              // think about damaging through time
		void think_flag();				  // think method for flag animation

		int getSmokeChance();             // probability to create smoke particle

		void die();                       // die

		// drawing
		int iDrawX();
		int iDrawY();

		// Functionality
		int getNonOccupiedCellAroundStructure();

		// getters

		// convenience get method, which should eventually only be used in rare cases as all properties should be
		// TODO: copied from this struct to this class, so each structure can change the properties without
		// changing the entire game rules.
		s_Structures getS_StructuresType() const;
		cPlayer * getPlayer();

		// Really sure you need this? You can use getPlayer() ?
		int getPlayerId() const { return iPlayer; }

		BITMAP * getBitmap();
		BITMAP * getShadowBitmap();

		int pos_x();
		int pos_y();

		bool isDead() { return dead; } // if set, then it is elegible for cleanup

		int getArmor() { return armor; }
		int getWidth() { return iWidth; }
		int getHeight() { return iHeight; }
		int getWidthInPixels();
		int getHeightInPixels();
		int getHitPoints() { return iHitPoints; }
		int getCell() { return iCell; }
		int getOwner() { return iPlayer; } // return the player id who owns this structure
		int getRallyPoint() { return iRallyPoint; }
		int getFrame() { return iFrame; }
		int getStructureId() { return id; }
		int getMaxHP();
		int getCaptureHP();
		int getSight();
		int getRange();
		int getPercentageNotPaved();
		int getPowerUsage();
		int getBuildingFase() { return iBuildFase; }

		bool isAnimating() { return bAnimate; }
		bool isPrimary();
		bool isRepairing() { return bRepair; }
		bool hasUnitWithin() const { return iUnitID > -1; }
		int getUnitIdWithin() const { return iUnitID; }
		bool isValid();
		bool canAttackAirUnits() const { return getS_StructuresType().canAttackAirUnits; }


		void setHeight(int height);
		void setWidth(int width);
		void setHitPoints(int hp);
		void setCell(int cell);
		void setOwner(int player); // set owner (player id) of structure
		void setRallyPoint(int cell); // set rally point of structure
		void setAnimating(bool value); // set animation on / off
		void setFrame(int frame);
		void setStructureId(int theId) { id = theId; }
		void setBuildingFase(int value) { iBuildFase = value; }
		void setRepairing(bool value);

		void damage(int hp); // damage structure for x amount of hp
    	float getHealthNormalized();

    	bool isDamaged();

        bool belongsTo(int playerId) const;
        bool belongsTo(const cPlayer * other) const;

        int getRandomPosX();
        int getRandomPosY();

    void getsCapturedBy(cPlayer *pPlayer);
};

#endif
