/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2009 (c) code by Stefan Hendriks


  ---
  New
  ---

  Giving a go for true OOP programming, using a base class and derived classes.
  */



class cAbstractStructure {

	private:
		int iHitPoints;     // HitPoints this structure has

		int iCell;          // What cell it is (most upper left part of structure where
							// drawing starts)

		int iPlayer;        // owner
	
		int iRallyPoint;    // rallypoint ...

		bool bAnimate;      // Do its special animation? (unit leaving building, starport
							// dropping something, etc)

	protected:
		int iWidth;			// width in cells (set by factory)
		int iHeight;		// height in cells (set by factory)

		int iFrame;          // Frame for animation (flag and other)

    public:

		// Constructor & Destructor:
		cAbstractStructure(); // default constructor (never used actually!)
	    
		cAbstractStructure(int iID);
		~cAbstractStructure();
	    
		
	    
		float fConcrete;     // how much concrete is *not* beneath this building (percentage)?
							 // meaning, when 0% , it is all concrete. But if 10%, it means 10% of the building
							 // is not covered.

		// Repairing stuff
		bool bRepair;       // repairing? (using timer + gives animation)
		int iRepairY;		// raising repair thingy
		int iRepairX;		// repair X position (changes everytime?)
		int iRepairAlpha;	// repair alpha

		int iUnitID;        // >-1 means ID to unit

		// Building animation
		int iBuildFase;

		// TIMERS that all structures use
		int TIMER_repair;   // repairing timer
		int TIMER_repairanimation; // repair animation timer
		int TIMER_flag;     // flag animation
		int TIMER_fade;     // fading timer

		int TIMER_damage;   // damaging stuff
		int TIMER_prebuild; // prebuild timer

		// -------------
	    


		// Filled in by derived classes 
		virtual void draw(int iStage)=0;              // draw
		virtual void think()=0;           // think
		virtual void think_animation()=0; // think animation stuff        
		virtual void think_guard();       // think guard stuff
		
		virtual int getType()=0;					  // implementation gives type of structure
		void think_prebuild();            // prebuild animation
		void think_repair();              // repair thinking
		void think_damage();              // think about damaging through time
		void think_flag();				  // think method for flag animation

		void die();                       // die

		// drawing
		int iDrawX();
		int iDrawY();
	    
		// Functionality
		int iFreeAround();

		// getters
		int getWidth() { return iWidth; } 
		int getHeight() { return iHeight; }
		int getHitPoints() { return iHitPoints; }
		int getCell() { return iCell; }
		int getOwner() { return iPlayer; } // return the player id who owns this structure
		int getRallyPoint() { return iRallyPoint; }
		int getFrame() { return iFrame; }

		bool isAnimating() { return bAnimate; }

		void setHeight(int height);
		void setWidth(int width);
		void setHitPoints(int hp);
		void setCell(int cell);
		void setOwner(int player); // set owner (player id) of structure
		void setRallyPoint(int cell); // set rally point of structure
		void setAnimating(bool value); // set animation on / off
		void setFrame(int frame);

		void damage(int hp); // damage structure for x amount of hp
};

void upgradeTechTree(int iPlayer, int iStructureType);
int STRUCTURE_FREE_TYPE(int iPlyr, int iCll, int iTpe);


