/*

 Dune II - The Maker

 Author : Stefan Hendriks
 Contact: stefanhen83@gmail.com
 Website: http://dune2themaker.fundynamic.com

 2001 - 2011 (c) code by Stefan Hendriks

 */

// Define TRANSFER stuff for reinforcements
#define TRANSFER_NONE	-1				// nothing to transfer
#define TRANSFER_NEW_STAY	0			// bring a new unit, and let the carryall stay
#define TRANSFER_NEW_LEAVE	2			// bring a new unit, and let the carryall go back (and die)
#define TRANSFER_PICKUP		1			// carry an existing unit, bring to new location
#define TRANSFER_DIE		3			// will die when reaching goal-cell (added for proper animation)
#define ACTION_GUARD        0           // on guard (scanning for enemy activitiy)
#define ACTION_MOVE         1           // moving
#define ACTION_CHASE        2           // chasing a unit to attack
#define ACTION_ATTACK       3           // attacking (not moving)
// Reinforcement data (loaded from ini file)
struct sReinforcement {
		int iSeconds; // FPS based, every second this decreases (0 = deliver)
		int iUnitType; // what unit?
		int iPlayer; // to who?
		int iCell; // Where to?
};

class cUnit {
	public:

		float fExperience; // experience gained by unit

		int iID;

		int iCell; // cell of unit

		int iType; // type of unit

		// Use char, max groups = 5, we do not need even an int! (byte is enough!)
		char iGroup; // belongs to group...

		int iHitPoints; // hitpoints of unit
		int iTempHitPoints; // temp hold back for 'reinforced' / 'dumping' and 'repairing' units

		int iPlayer; // belongs to player

		// Movement
		int iNextCell; // where to move to (next cell)
		int iGoalCell; // the goal cell (goal of path)
		int iCellX; // my cell x
		int iCellY; // my cell y
		float iOffsetX; // X offset
		float iOffsetY; // Y offset
		int iPath[MAX_PATH_SIZE]; // path of unit
		int iPathIndex; // where are we?
		int iPathFails; // failed...
		bool bCalculateNewPath; // calculate new path?

		// carryall stuff
		bool bCarryMe; // carry this unit when moving it around?
		int iCarryAll; // any carry-all that will pickup this unit... so this unit knows
		// it should wait when moving, etc, etc


		// WHEN ATTACKING
		int iAttackUnit; // attacking unit id
		int iAttackStructure; // attack structure id
		int iAttackCell; // attacking a cell (which is force attack)

		// Action its doing:
		int iAction; // ACTION_MOVE; ACTION_GUARD; ACTION_CHASE;

		// Action given code
		int iUnitID; // Unit ID to attack/pickup, etc
		int iStructureID; // structure ID to attack/bring to (refinery)

		// Harvester specific
		int iCredits; // credits stored in this baby

		// Carry-All specific
		int iTransferType; // -1 = none, 0 = new (and stay), 1 = carrying existing unit , 2 = new (and leave)
		// iUnitID = unit we CARRY (when TransferType == 1)
		// iTempHitPoints = hp of unit when transfertype = 1

		int iCarryTarget; // Unit ID to carry, but is not carried yet
		int iBringTarget; // Where to bring the carried unit (when iUnitID > -1)
		int iNewUnitType; // new unit that will be brought, will be this type
		bool bPickedUp; // picked up the unit?

		// Drawing
		int iBodyFacing; // Body of tanks facing
		int iHeadFacing; // Head of tanks facing
		int iBodyShouldFace; // where should the unit body look at?
		int iHeadShouldFace; // where should the unit look at?
		int iFrame; // framed (animated stuff)

		// selected
		bool bSelected; // selected or not?

		float fExpDamage(); // experience damage by bullet (extra damage that is)

		// ------------
		void init(int i); // inits units
		int draw_x();
		int draw_y();
		void draw_health();
		void draw_experience();
		void draw_spice();
		void draw();
		void draw_path();
		bool isValid(); // valid unit?

		void shoot(int iShootCell); // shoot at goalcell

		void die(bool bBlowUp, bool bSquish); // die!

		void poll(); // poll status

		void think(); // thinking in general
		void think_move_air(); // aircraft specific
		void think_move_foot(); // soldiers specific
		void think_move(); // thinking about movement (which is called upon a faster rate)
		// for wheeled/tracked units
		void think_attack();
		void think_guard();

		void LOG(const char *txt);

		void think_hit(int iShotUnit, int iShotStructure);

		int is_nextcell(); // what is the next cell to move to

		void move_to(int iCll, int iStrucID, int iUnitID);

		// carryall-functions:
		void carryall_order(int iuID, int iTransfer, int iBring, int iTpe);

		// -------------
		int TIMER_blink; // blink

		int TIMER_move; // movement timer
		int TIMER_movewait; // wait for move think...

		int TIMER_thinkwait; // wait with normal thinking..

		int TIMER_turn; // turning around
		int TIMER_frame; // frame

		int TIMER_harvest; // harvesting

		int TIMER_guard; // guard scanning timer
		int TIMER_bored; // how long are we bored?

		int TIMER_attack; // when to shoot?

		int TIMER_wormeat; // when do we eat? (when worm)
};

void SPAWN_FRIGATE(int iPlr, int iCll);

int UNIT_CREATE(int iCll, int iTpe, int iPlyr, bool bOnStart);
int CREATE_PATH(int iID, int iPathCountUnits);
int RETURN_CLOSE_GOAL(int iCll, int iMyCell, int iID);

void UNIT_deselect_all();
void UNIT_ORDER_MOVE(int iUnitID, int iGoalCell);
void UNIT_ORDER_ATTACK(int iUnitID, int iGoalCell, int iUnit, int iStructure, int iAttackCell);

int UNIT_find_harvest_spot(int id);
void REINFORCE(int iPlr, int iTpe, int iCll, int iStart);
int CARRYALL_TRANSFER(int iuID, int iGoal);

int UNIT_FREE_AROUND_MOVE(int iUnit);

void THINK_REINFORCEMENTS();
void SET_REINFORCEMENT(int iCll, int iPlyr, int iTime, int iUType);
void INIT_REINFORCEMENT();

int CLOSE_SPICE_BLOOM(int iCell);

