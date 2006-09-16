/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@dune2k.com / stefanhen83@planet.nl
  Website: http://d2tm.duneii.com

  2001 - 2006 (c) code by Stefan Hendriks

  ---
  New
  ---

  Giving a go for true OOP programming, using a base class and derived classes.
  */



class cStructure
{
    public:

    // Constructor & Destructor:
    cStructure(); // default constructor (never used actually!)
    
    cStructure(int iID);
    ~cStructure();

    int iHitPoints;      // HitPoints this structure has
    int iCell;           // What cell it is (most upper left part of structure where
                         // drawing starts)
    float fConcrete;     // how much concrete is beneath this building (percentage)?    
    int iPlayer;         // belongs to...
    int iFrame;          // Frame (flag animation)
    int iType;           // Type of structure to identify

    // Repairing stuff
    bool bRepair;       // repairing? (using timer + gives animation)
	int iRepairY;		// raising repair thingy
	int iRepairX;		// repair X position (changes everytime?)
	int iRepairAlpha;	// repair alpha

    // Other stuff
    bool bAnimate;      // Do its special animation? (unit leaving building, starport
                        // dropping something, etc)

    int iRallyPoint;    // rallypoint ...

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
    
    // Width and Height in cells (preset by constructor of type)
    int iWidth;
    int iHeight;

    // Filled in by derived classes    
    virtual void draw(int iStage)=0;              // draw
    virtual void think()=0;           // think
    virtual void think_animation()=0; // think animation stuff        
    virtual void think_guard();       // think guard stuff
    void think_prebuild();            // prebuild animation
    void think_repair();              // repair thinking
    void think_damage();              // think about damaging through time

    void die();                     // die

    // drawing
    int iDrawX();
    int iDrawY();
    
    // Functionality
    int iFreeAround();

    private:
    
};

int STRUCTURE_FREE_TYPE(int iPlyr, int iCll, int iTpe);
int STRUCTURE_VALID_LOCATION(int iCll, int iTpe, int iUnitID);
int STRUCTURE_CREATE(int iCll, int iTpe, int iHP, int iPlyer);


