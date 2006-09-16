/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@dune2k.com / stefanhen83@planet.nl
  Website: http://d2tm.duneii.com

  2001 - 2006 (c) code by Stefan Hendriks

  */

// TODO: Clean this class up big time.

// List structure
struct tListIcon
{
	int iIcon;
	int iStructureID;		// any id attached
	int iUnitID;			// any id attached
	int iPrice;				// price of this thing	
};


class cGame
{

public:

	bool windowed;				// windowed
	char version[15];			// version number, or name.

    int iGameTimer;             // game timer

    // Alpha (for fading in/out)
    int iAlphaScreen;           // 255 = opaque , anything else
    int iFadeAction;            // 0 = NONE, 1 = fade out (go to 0), 2 = fade in (go to 255)

    // World stuff
    // 1. Show current conquered regions
    // 2. Show next progress + story (in message bar)
    // 3. Click next region
    // 4. Set up region and go to GAME_BRIEFING, which will do the rest...-> fade out

    int iRegionState;           // 
    int iRegionScene;           // scene
    int iRegionSceneAlpha;           // scene alpha
    int iRegionConquer[MAX_REGIONS];     // INDEX = REGION NR , > -1 means conquered..
    int iRegionHouse[MAX_REGIONS];
    char cRegionText[MAX_REGIONS][255]; // text for this

    // resolution of the game
	int screen_x;
	int screen_y;

    bool bPlaySound;            // play sound?
    bool bMp3;                  // use mp3 files instead of midi

	bool bPlaying;				// playing or not
    bool bSkirmish;             // playing a skirmish game  or not 
	int  iSkirmishMap;			// what map is selected
	int screenshot;				// screenshot taking number
	int iSkirmishStartPoints;	// random map startpoints

	void init();		// initialize all game variables
	void mission_init(); // initialize variables for mission loading only
	void run();			// run the game

	bool bMousePressedLeft, bMousePressedRight;
	bool mouse_left, mouse_right;

	int map_width;
	int map_height;

    int iRegion;        // what region is selected?
	int iMission;		// what mission are we playing? (= techlevel)
	int iHouse;			// what house is selected for playing? 

	int selected_structure;
	int hover_structure;
	int hover_unit;

	int fade_select;        // fade color when selected
    bool bFadeSelectDir;    // fade select direction

	int iActiveList;		// what list is active to view? (refering to ID of list)

	int paths_created;

	// MESSAGING / HELPER
	char cMessage[255];
	int iMessageAlpha;
	int TIMER_message;

    int iMouseZ;
    int iSoundsPlayed;

    int iMusicVolume;       // volume of the mp3 / midi

    // Movie playback (scene's from datafile)
    int TIMER_movie;
    int iMovieFrame;

    // throttle stuff
	int throttle_x;
    int throttle_y;
    int TIMER_throttle;

    int TIMER_money;
    int iCountSoundMoney;

    int iMusicType;

	// Scrolling around map, timer based
	int TIMER_scroll;
	int iScrollSpeed;

    // Condition to win the mission:
    int iWinQuota;              // > 0 means, get this to win the mission, else, destroy all!
    
	// Starport stuff
	int TIMER_starport;			// timer for refreshing list
	int TIMER_ordered;			// timer for ordering stuff ( -1 = nothing, 0 = delivering, > 0 t-minus)
	int TIMER_mayorder;			// timer > 0 , then we may not order!

	// List stuff
	tListIcon iconlist[LIST_MAX][MAX_ICONS];
	int iconscroll[LIST_MAX];
	int iconbuilding[LIST_MAX];	// what are we building?
	int iconTIMER[LIST_MAX];	// timer for building
	int iconprogress[LIST_MAX];	// progress done on this icon?
    int iLastBuilt[LIST_MAX];   // last type built per list

	
	// Frigate stuff
	int iconFrigate[MAX_ICONS]; // what is ordered? (how many of them?)

	// Upgrade stuff
	int iStructureUpgrade[MAX_STRUCTURETYPES];
	int iUpgradeTIMER[MAX_STRUCTURETYPES];
	int iUpgradeProgress[MAX_STRUCTURETYPES];
	int iUpgradeProgressLimit[MAX_STRUCTURETYPES]; // set limit for upgrade

    void think_winlose(); // think about winning/losing mission
    void winning();       // winning
    void losing();        // losing

	// Building stuff, requires a think function
	void think_build();	// building stuff from lists
	void think_upgrade(); // upgrading stuff
	void think_money();	// think about upping/downing money

	void think_starport();

    void think_movie();

	bool bPlaceIt;		// placing something? (for structures only)
	bool bPlacedIt;		// for remembering, for combat_mouse stuff..

	void list_insert_item(int iListID, int iIcon, int iPrice, int iStructureID, int iUnitID, int iPosition);
	void list_new_item(int iListID, int iIcon, int iPrice, int iStructureID, int iUnitID);

	void setup_list();		// set up all lists at the beginning of any mission (depening on tech level, house, etc)
	void think_message();
	void set_message(char msg[255]); // new message

    void think_music();

	char mentat_sentence[10][255];		// draw 2 sentences at once, so 0 1, 2 3, 4 5, 6 7, 8 9

	int TIMER_mentat_Speaking;	// speaking = time

	int iMentatSpeak;			// = sentence to draw and speak with (-1 = not ready)
	int iMentatMouth;			// frames	...
	int iMentatEyes;				// ... for mentat ...
	int iMentatOther;			// ... animations .

	int TIMER_mentat_Mouth;			// timer for animations
	int TIMER_mentat_Eyes;			// timer for animations
	int TIMER_mentat_Other;			// timer for animations

	void MENTAT_draw_mouth(int iMentat);
	void MENTAT_draw_eyes(int iMentat);
	void MENTAT_draw_other(int iMentat);

	void think_mentat();

    void FADE_OUT(); // fade out with current screen_bmp, this is a little game loop itself!

	void preparementat(bool bTellHouse);

	bool setupGame();
	void shutdown();
	
	bool isState(int thisState);
	void setState(int thisState);

private:
	void poll();
	void gerald();		// interface and such, which is called by combat
	void combat();		// the combat part (main) of the game
	
	void draw_mentat(int iType); // draw mentat type

    void setup_skirmish();  // set up a skirmish game
	void house();		// house selection
	void tellhouse();	// tell about the house
	void mentat(int iType);		// mentat talking
	void menu();		// main menu
	void region();		// region selection

	void combat_mouse();
	void mapdraw();
	void draw_credits();  // draw money
	void draw_sidebarbuttons();	// draw the buttons left to the icon bar (and show what is active)
	void draw_placeit();
	void draw_order();

	void runGameState();
	void shakeScreenAndBlitBuffer();
	void handleTimeSlicing();
	void handleKeys();

    void draw_movie(int iType);  // draw movie sequence


	void draw_message();
	

	// upgrade button drawing and behaviour
	void draw_upgrade();

	/** game state **/
	int state;

	// List specific
	void draw_list();
	void init_lists();
	int  list_new_id(int iListID);		// find new entry in this list

};