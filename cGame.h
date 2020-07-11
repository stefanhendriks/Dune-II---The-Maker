/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2010 (c) code by Stefan Hendriks

  */

// TODO: Clean this class up big time.


class cGame {

public:

	cGame();

	std::string game_filename;

	bool windowed;				// windowed
	char version[15];			// version number, or name.

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

	int map_width;
	int map_height;

    int iRegion;        // what region is selected?
	int iMission;		// what mission are we playing? (= techlevel)
	int iHouse;			// what house is selected for playing?

	int selected_structure;
	int hover_unit;

	int fade_select;        // fade color when selected
    bool bFadeSelectDir;    // fade select direction

	int paths_created;

    int iMusicVolume;       // volume of the mp3 / midi

    // Movie playback (scene's from datafile)
    int TIMER_movie;
    int iMovieFrame;

    // throttle stuff
	int shake_x;
    int shake_y;
    int TIMER_shake;

    int iMusicType;

    // Condition to win the mission:
    int iWinQuota;              // > 0 means, get this to win the mission, else, destroy all!

    void think_winlose();
    void winning();       // winning
    void losing();        // losing

    void think_movie();

	bool bPlaceIt;		// placing something? (for structures only)
	bool bPlacedIt;		// for remembering, for combat_mouse stuff..

	void setup_players();
	void think_message();

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

	int getMaxVolume() { return iMaxVolume; }

	cSoundPlayer * getSoundPlayer() { return soundPlayer; }

	void draw_placeit();
	void combat_mouse();

	int getGroupNumberFromKeyboard();
	void destroyAllUnits(bool);
	void destroyAllStructures(bool);

private:
	void poll();
	void combat();		// the combat part (main) of the game

	void draw_mentat(int iType); // draw mentat type

    void setup_skirmish();  // set up a skirmish game
	void house();		// house selection
	void tellhouse();	// tell about the house
	void mentat(int iType);		// mentat talking
	void menu();		// main menu
	void region();		// region selection

	void runGameState();
	void shakeScreenAndBlitBuffer();
	void handleTimeSlicing();

    void draw_movie(int iType);  // draw movie sequence

	/** game state **/
	int state;

	int iMaxVolume;

	cSoundPlayer *soundPlayer;
};
