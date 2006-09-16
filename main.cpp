/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@dune2k.com / stefanhen83@planet.nl
  Website: http://d2tm.duneii.com

  2001 - 2006 (c) code by Stefan Hendriks

  */

#include "d2tmh.h" 

bool bDoDebug = false;
int	iRest = 1;	// rest value

// Game settings
int iIdealFPS = 60;           // Ideal FPS = 60

// Server/Client system

// the ultimate game variable(s)
cGame          game;
cStructure     *structure[MAX_STRUCTURES];
cUnit          unit[MAX_UNITS];
cMap           map;
cPlayer        player[MAX_PLAYERS];             // player is 
cAIPlayer      aiplayer[MAX_PLAYERS];           // related to aiplayer (except nr 0=human)
cParticle      particle[MAX_PARTICLES];
cBullet        bullet[MAX_BULLETS];
cRegion        world[MAX_REGIONS];
cMentat		   *mentat;

// Structs
sPreviewMap     PreviewMap[MAX_SKIRMISHMAPS];        // max of 100 maps in skirmish directory
s_House         houses[MAX_HOUSES];
s_Structures    structures[MAX_STRUCTURETYPES];    // structure types
s_UnitP         units[MAX_UNITTYPES];              // unit types
s_Bullet        bullets[MAX_BULLET_TYPES];          // bullet slots
sReinforcement  reinforcements[MAX_REINFORCEMENTS];  // reinforcement slots

int MAXVOICES = 8;            // USE THIS FOR DETERMINING VOICES

// palette
PALETTE general_palette;

// bitmap(s)
BITMAP *bmp_screen;
BITMAP *bmp_throttle;
BITMAP *bmp_winlose;
BITMAP *bmp_fadeout;

// datafile(s)
DATAFILE *gfxdata;		// graphics (terrain, units, structures)
DATAFILE *gfxaudio;		// audio
DATAFILE *gfxinter;		// interface graphics
DATAFILE *gfxworld;		// world/pieces graphics
DATAFILE *gfxmentat;	// mentat graphics
DATAFILE *gfxmovie;     // movie to be played (= scene = data file)

// FONT stuff
ALFONT_FONT *game_font; // arrakeen.fon
ALFONT_FONT *bene_font;	// benegesserit font.

// MP3 STUFF
ALMP3_MP3   *mp3_music; // pointer to mp3 music

// TIMER fixes (do not run the same timer 2x, skip it)
volatile int RUNNING_TIMER_units = 0;
volatile int RUNNING_TIMER_tenth = 0;
volatile int RUNNING_TIMER_fps = 0;

// Mouse information
int mouse_co_x1;      // coordinates
int mouse_co_y1;      // of
int mouse_co_x2;      // the
int mouse_co_y2;      // mouse border

int mouse_status;     // status of the mouse (see main.h)
int mouse_tile;       // mouse picture in gfxdata

// Timers
int frame_count, fps;  // fps and such 

/***
  Timer    : This timer is called for units and structures
  Purpose  : Activates think functions
  ***/
void timer_units()
{
  RUNNING_TIMER_units++;
}
END_OF_FUNCTION(timer_units);


/***
  Timer    : This timer is called very 0.1 second
  Purpose  : Activates think functions
  ***/
void timer_tenth()
{    
  RUNNING_TIMER_tenth++;
}

END_OF_FUNCTION(timer_tenth);


/***
  Timer    : This timer is called very second
  Purpose  : timer callback for measuring the frames per second
  ***/
void fps_proc(void)
{  
  RUNNING_TIMER_fps++;
}
END_OF_FUNCTION(fps_proc);



/* Processes timer functions */
void TIME_process()
{   
	// fix any super high timers
	if (RUNNING_TIMER_fps > 10)
	{
		if (DEBUGGING)	logbook("WARNING: Exeptional high RUNNING_TIMER_FPS;  fixed at 10");
		RUNNING_TIMER_fps = 10;	// cap at 10
	}

	if (RUNNING_TIMER_tenth > 40)
	{
		if (DEBUGGING)	logbook("WARNING: Exeptional high RUNNING_TIMER_TENTH; fixed at 40");
		RUNNING_TIMER_tenth = 40; // cap at 40
	}

	if (RUNNING_TIMER_units > 20)
	{
		if (DEBUGGING)	logbook("WARNING: Exeptional high RUNNING_TIMER_UNITS; fixed at 20");
		RUNNING_TIMER_units = 20; // cap at 20
	}

	// FPS timer processing
	while (RUNNING_TIMER_fps > 0)
	{
		/* Path creation limitation */

		// and the clock is ticking (game timer)
		game.iGameTimer++;

		if (game.state == GAME_PLAYING)
		{
			game.paths_created = 0;
			game.iSoundsPlayed=0;
			game.think_starport(); // think about lists for starport

			THINK_REINFORCEMENTS();
			
			// starports think per second for deployment (if any)
			for (int i =0; i < MAX_STRUCTURES; i++)
				if (structure[i])
					if (structure[i]->iType == STARPORT)
						((cStarPort *)structure[i])->think_deploy(); 


        } // game specific stuff
         

		// Frame Per Second counter
		fps = frame_count;

        

		// 'auto resting'
		if (fps < iIdealFPS)
		{
			// reduce with 10
			if (iRest > 0)
				iRest-=2;

			if (iRest < 0)
				iRest=0;

			char msg[255];
			sprintf(msg, "AUTO-REST: FPS is lower then ideal (=%d), reducing rest value", iIdealFPS);
			logbook(msg);
		}
		else
		{
			// we go fast enough, so increase rest
			if (iRest < 500)
				iRest+=2;

			if (iRest > 500)
				iRest=500;

			char msg[255];
			sprintf(msg, "AUTO-REST: FPS is higher then ideal (=%d), increasing rest value", iIdealFPS);
			logbook(msg);
		}


		// log the status
		frame_count = 0;   

		RUNNING_TIMER_fps--; // done!
	}

  
	// Unit timer
	while (RUNNING_TIMER_units > 0)
	{

		if (game.state == GAME_PLAYING)
		{

			// units think
			for (int i=0; i < MAX_UNITS; i++)
				if (unit[i].isValid())
				{ 
					unit[i].think();

					// Think attack style
					if (unit[i].iAction == ACTION_ATTACK)
						unit[i].think_attack();          
				}

				map.think_minimap();
		}  
		RUNNING_TIMER_units--;
	}

	// keep up with time cycles
	// USED FOR UNITS
	while (RUNNING_TIMER_tenth > 0)
	{
		if (game.iFadeAction == 1)
		{
			game.iAlphaScreen-=2;
			if (game.iAlphaScreen < 0)
			{
				game.iAlphaScreen = 0;
				game.iFadeAction=0;
			}        
		}
		else if (game.iFadeAction == 2)
		{
			game.iAlphaScreen+=2;
			if (game.iAlphaScreen > 255)
			{
				game.iAlphaScreen = 255;
				game.iFadeAction=0;
			}
		}
		game.think_music();
		game.think_mentat();
		game.think_movie();
		game.think_message();

		// THINKING ONLY WHEN PLAYING / COMBAT
		if (game.state == GAME_PLAYING)
		{	

			// structures think
			for (int i=0; i < MAX_STRUCTURES; i++)
				if (structure[i])
				{
					structure[i]->think();           // think about actions going on
					structure[i]->think_animation(); // think about animating
					structure[i]->think_guard();     // think about 'guarding' the area (turrets only)
				}

				// DO NOT THINK FOR HUMAN PLAYER (== 0)
				for (int i=0; i < MAX_PLAYERS; i++)
					aiplayer[i].think();

				game.TIMER_scroll++;

				if (game.TIMER_scroll > game.iScrollSpeed)
				{
					map.think();
					game.TIMER_scroll=0;

				}
              
				game.think_build();
				game.think_upgrade();
				game.TIMER_money++;
                
				if (game.TIMER_money > 5)
				{
					game.think_money();
					game.TIMER_money=0;
				}


				if (game.TIMER_throttle > 0)
					game.TIMER_throttle--;



				// units think (move only)
				for (int i=0; i < MAX_UNITS; i++)
					if (unit[i].isValid())
					{
						// move
						if (unit[i].iAction == ACTION_MOVE || unit[i].iAction == ACTION_CHASE)
							unit[i].think_move();

						// guard
						if (unit[i].iAction == ACTION_GUARD)
							unit[i].think_guard();


						// move in air
						if (unit[i].iType == ORNITHOPTER &&
							unit[i].iAction == ACTION_ATTACK)
							unit[i].think_move_air(); // keep flying even when attacking


					}
					else
					{

					}


					for (int i=0; i < MAX_PARTICLES; i++)
						if (particle[i].isValid())
							particle[i].think();

					/*
					BEGIN_PROF("Players think");      
					for (i=0; i < MAX_PLAYERS; i++)        
					player[i].think(i);
					END_PROF();

					} // game playing
					*/      


					// when not drawing the options, the game does all it needs to do	
					// bullets think 
					for (int i=0; i < MAX_BULLETS; i++)
						if (bullet[i].bAlive)
							bullet[i].think(); 


		}

		if (game.state == GAME_WINNING)
		{

		}

		// Fading of selected stuff
		if (game.bFadeSelectDir)
		{
			game.fade_select++;
			if (game.fade_select > 254)
				game.bFadeSelectDir=false;
		}
		else
		{
			game.fade_select--;
			if (game.fade_select < 32)
				game.bFadeSelectDir = true;
		}

		RUNNING_TIMER_tenth--;
  }
} // TIME PROCESSING

/***
  Function : install()
  Returns  : TRUE on succes, FALSE on failure
  Purpose  : Will install game variables, sets up the entire game.
***/
bool install()
{ 	
	game.init(); // Must be first!

	// Each time we run the game, we clear out the logbook
	FILE *fp;
	fp = fopen("log.txt", "wt");

	if (fp) 
	{
		fprintf(fp, "DUNE II - The Maker\n");
		fprintf(fp, "-------------------\n\n"); // print head of logbook
		fclose(fp);
	}

	logbook("-------------------");
	logbook("Version information");
	logbook("-------------------");
	logbook(game.version);
	char msg[255];
	sprintf(msg, "Compiled at %s , %s", __DATE__, __TIME__);
	logbook(msg);   

	if (DEBUGGING) logbook("DEBUG MODE ENABLED");

	// init game
	if (game.windowed)
		logbook("Windowed mode");
	else
		logbook("Fullscreen mode");



	mouse_co_x1 = -1;      // coordinates
	mouse_co_y1 = -1;      // of
	mouse_co_x2 = -1;      // the
	mouse_co_y2 = -1;      // mouse border

	// TODO: load eventual game settings (resolution, etc)


	// Logbook notification
	logbook("\n-------");
	logbook("Allegro");
	logbook("-------");

	// ALLEGRO - INIT    
	if (allegro_init() != 0)
		return false;

	logbook(allegro_id);
	yield_timeslice();

	logbook("yield_timeslice()");

	int r = install_timer();
	if (r > -1) logbook("install_timer()");
	else
	{
		allegro_message("Failed to install timer");
		logbook("FAILED");
		return false;
	}	

	alfont_init();
	logbook("alfont_init()");
	install_keyboard();
	logbook("install_keyboard()");
	install_mouse();
	logbook("install_mouse()");

	logbook("setting up timer functions / locking functions & memory");
	/* set up the interrupt routines... */  
	LOCK_VARIABLE(RUNNING_TIMER_units);
	LOCK_VARIABLE(RUNNING_TIMER_tenth);  
	LOCK_VARIABLE(RUNNING_TIMER_fps);  
	LOCK_FUNCTION(timer_units);  
	LOCK_FUNCTION(timer_tenth);  
	LOCK_FUNCTION(fps_proc);  

	game.TIMER_money=0;
	game.TIMER_message=0;
	game.TIMER_throttle=0;

	//install_int(timer_units, 100);
	//install_int(timer_tenth, 10);
	install_int(timer_units, 100);
	install_int(timer_tenth, 5);  
	install_int(fps_proc, 1000);

	logbook("Timers installed");

	frame_count = fps = 0;

	// set window title
	char title[128];
	sprintf(title, "Dune II - The Maker [%s] - (by Stefan Hendriks)", game.version);

	// Set window title
	set_window_title(title);  
	char window_title[256];
	sprintf(window_title, "Window title set: [%s]", title);
	logbook(window_title);

	set_window_close_button(0);


	set_color_depth(16);



	//if (iDepth > 15 && iDepth != 24)
	//set_color_depth(iDepth);

	if (game.windowed)
	{
		/*
		int 	iDepth = desktop_color_depth();

		// dont switch to 15 bit or lower, or at 24 bit
		if (iDepth > 15 && iDepth != 24)
		{
		char msg[255];
		sprintf(msg,"DESKTOP: Found desktop color depth. Will switch to %d bit.", iDepth);
		logbook(msg);
		set_color_depth(iDepth);      // run in the same bit depth as the desktop
		}
		else
		{
		// default color depth is 16
		logbook("DESKTOP: No bit depth autodetected. Will switch to default, 16 bit.");
		set_color_depth(16);
		}*/
		//GFX_AUTODETECT_WINDOWED
		int r = set_gfx_mode(GFX_AUTODETECT_WINDOWED, game.screen_x, game.screen_y, game.screen_x, game.screen_y);  
		if (r > -1)
		{
			// Succes      
		}
		else
		{      
			// GFX_DIRECTX_ACCEL / GFX_AUTODETECT
#ifdef UNIX
			r = set_gfx_mode(GFX_XWINDOWS, game.screen_x, game.screen_y, game.screen_x, game.screen_y);
#else
			r = set_gfx_mode(GFX_DIRECTX_ACCEL, game.screen_x, game.screen_y, game.screen_x, game.screen_y);
#endif

			if (r > -1)
			{  
				game.windowed = false;

				/*
				FILE *f;
				f = fopen("settings.d3", "wb");

				fwrite(&game.play_music , sizeof(bool)    ,1 , f);        
				fwrite(&game.play_sound , sizeof(bool)    ,1 , f);
				fwrite(&game.fade , sizeof(bool)    ,1 , f);
				fwrite(&game.windowed , sizeof(bool)    ,1 , f);  
				fwrite(&game.screen_x , sizeof(int)    ,1 , f);            
				fwrite(&game.screen_y , sizeof(int)    ,1 , f);            
				fclose(f);          
				logbook("Could not enter windowed-mode; settings.d3 adjusted"); */

			}
			else
			{        
				allegro_message("Failed to set resolution");
				return false;
			}
		}
	}
	else
	{

		int r = set_gfx_mode(GFX_AUTODETECT, game.screen_x, game.screen_y, game.screen_x, game.screen_y);

		// succes
		if (r > -1)
		{

		}
		else
		{
			return false;
		}

	}


	text_mode(-1);
	alfont_text_mode(-1);


	logbook("Loading font data");
	// loading font

	game_font = alfont_load_font("data/arakeen.fon");  

	if (game_font != NULL)
		alfont_set_font_size(game_font, GAME_FONTSIZE); // set size
	else
	{
		allegro_message("Failed to load arakeen.fon");
		return false;
	}


	bene_font = alfont_load_font("data/benegess.fon");  

	if (bene_font != NULL)
		alfont_set_font_size(bene_font, 10); // set size
	else
	{
		allegro_message("Failed to load benegess.fon");
		return false;
	}



	if (set_display_switch_mode(SWITCH_BACKGROUND) < 0)
	{
		set_display_switch_mode(SWITCH_PAUSE);
		logbook("Display 'switch and pause' mode set");
	}
	else
		logbook("Display 'switch to background' mode set");



	// sound
	logbook("Initializing sound");
	bool bSucces = false;
	int voices = 32;
	while (1) {
		if (voices <= 4) {
			break;
		}

		reserve_voices(voices, 0);
		if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) == 0) 
		{
			char msg[255];
			sprintf(msg, "Succes with %d reserved voices.", voices);
			logbook(msg);
			MAXVOICES=voices;
			bSucces=true;            
			break;
		}
		else {
			voices /= 2;
		}
	}

	if (!bSucces) {
		char msg[255];
		sprintf(msg, "%s", allegro_error);
		logbook(msg); 
		logbook("ERROR: Cannot initialize sound card");
	}

	/***
	Bitmap Creation
	***/

	bmp_screen = create_bitmap(game.screen_x, game.screen_y);

	if (bmp_screen == NULL)
	{
		allegro_message("Failed to create a memory bitmap");
		logbook("ERROR: Could not create bitmap: bmp_screen");
		return false;
	}
	else
	{
		logbook("Bitmap created: bmp_screen");
		clear(bmp_screen);
	}

	bmp_throttle = create_bitmap(game.screen_x, game.screen_y);

	if (bmp_throttle == NULL)
	{
		allegro_message("Failed to create a memory bitmap");
		logbook("ERROR: Could not create bitmap: bmp_throttle");
		return false;
	}
	else {
		logbook("Bitmap created: bmp_throttle");	
	}

	bmp_winlose = create_bitmap(game.screen_x, game.screen_y);

	if (bmp_winlose == NULL)
	{
		allegro_message("Failed to create a memory bitmap");
		logbook("ERROR: Could not create bitmap: bmp_winlose");
		return false;
	}
	else {
		logbook("Bitmap created: bmp_winlose");
	}

	bmp_fadeout = create_bitmap(game.screen_x, game.screen_y);

	if (bmp_fadeout == NULL)
	{
		allegro_message("Failed to create a memory bitmap");
		logbook("ERROR: Could not create bitmap: bmp_fadeout");
		return false;
	}
	else {
		logbook("Bitmap created: bmp_fadeout");
	} 

	/*** End of Bitmap Creation ***/
	set_color_conversion(COLORCONV_MOST);  

	logbook("Color conversion method set");

	// setup mouse speed
	set_mouse_speed(-1,-1);

	logbook("MOUSE: Mouse speed set");

	logbook("\n----");
	logbook("GAME ");
	logbook("----");

	/*** Data files ***/

	// load datafiles  
	gfxdata = load_datafile("data/gfxdata.dat");
	if (gfxdata == NULL) {
		logbook("ERROR: Could not hook/load datafile: gfxdata.dat");
		return false;
	} else {
		logbook("Datafile hooked: gfxdata.dat");
		memcpy (general_palette, gfxdata[PALETTE_D2TM].dat, sizeof general_palette);
	}

	gfxaudio = load_datafile("data/gfxaudio.dat");
	if (gfxaudio == NULL)  {
		logbook("ERROR: Could not hook/load datafile: gfxaudio.dat");
		return false;
	} else {
		logbook("Datafile hooked: gfxaudio.dat");
	}

	gfxinter = load_datafile("data/gfxinter.dat");
	if (gfxinter == NULL)  {
		logbook("ERROR: Could not hook/load datafile: gfxinter.dat");
		return false;
	} else {
		logbook("Datafile hooked: gfxinter.dat");
	}

	gfxworld = load_datafile("data/gfxworld.dat");
	if (gfxworld == NULL) {
		logbook("ERROR: Could not hook/load datafile: gfxworld.dat");
		return false;
	} else {
		logbook("Datafile hooked: gfxworld.dat");
	}

	gfxmentat = load_datafile("data/gfxmentat.dat");
	if (gfxworld == NULL) {
		logbook("ERROR: Could not hook/load datafile: gfxmentat.dat");
		return false;
	} else {
		logbook("Datafile hooked: gfxmentat.dat");	
	}

	gfxmovie = NULL; // nothing loaded at start. This is done when loading a mission briefing.

	DATAFILE *mp3music = load_datafile("data/mp3mus.dat");

	if (mp3music == NULL) {
		logbook("MP3MUS.DAT not found, using MIDI to play music");
		game.bMp3=false;
	} else {
		logbook("MP3MUS.DAT found, using mp3 files to play music");
		// Immidiatly load menu music
		game.bMp3=true;
	}

	// randomize timer

	srand((unsigned int) time(0));

	game.bPlaying = true;
	game.screenshot = 0;
	game.state = -1; 

	// Mentat class pointer set at null
	mentat = NULL;

	// Mouse stuff
	mouse_status = MOUSE_STATE_NORMAL;
	mouse_tile = 0;

	set_palette(general_palette);  

	// normal sounds are loud, the music is lower (its background music, so it should not be disturbing)
	set_volume(255, 150);

	// A few messages for the player
	logbook("Installing:  PLAYERS");
	INSTALL_PLAYERS();  
	logbook("Installing:  HOUSES");
	INSTALL_HOUSES();
	logbook("Installing:  STRUCTURES");
	install_structures();
	logbook("Installing:  BULLET TYPES");
	install_bullets();
	logbook("Installing:  UNITS");
	install_units();
	logbook("Installing:  WORLD");
	INSTALL_WORLD();

	logbook("'\n--------------");
	logbook("BATTLE CONTROL");
	logbook("--------------"); 
	logbook("\n3...2...1... GO!\n");

	game.init();
	game.setup_list();

	play_music(MUSIC_MENU);

	// all has installed well. Lets rock and role.
	return true;
}

void shutdown() {
	// Destroy font of Allegro FONT library
	alfont_destroy_font(game_font);
	alfont_destroy_font(bene_font);

	logbook("\n--------");
	logbook("SHUTDOWN");
	logbook("--------");

	// Exit the font library (must be first)

	alfont_exit();
	logbook("Allegro FONT library shut down.");

	if (mp3_music != NULL)
	{
		almp3_stop_autopoll_mp3(mp3_music); // stop auto poll
		almp3_destroy_mp3(mp3_music);
	}

	logbook("Allegro MP3 library shut down.");

	// Now we are all neatly closed, we exit Allegro and return to OS hell.
	allegro_exit();
	logbook("Allegro shut down.");
	logbook("\nThanks for playing!");
}


/*
Function: main()
Purpose : make sure the darn thing works.
  */
int main() {
 
 if (install()) {   
   game.run();
 }

 shutdown();
 return 0;
}
END_OF_MAIN();
