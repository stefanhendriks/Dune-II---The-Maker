/*

 Dune II - The Maker

 Author : Stefan Hendriks
 Contact: stefanhen83@gmail.com
 Website: http://dune2themaker.fundynamic.com

 2001 - 2011 (c) code by Stefan Hendriks

 */

#include <string>


#include "domain/Mouse.h"

#include "utils/Logger.h"

#include "states/MainMenuState.h"
#include "cGame.h"

#include "include/data/gfxdata.h"
#include "include/allegroh.h"

using namespace std;

bool bDoDebug = false;

// palette
PALETTE general_palette;

// bitmap(s)
BITMAP *bmp_screen;
// BITMAP *bmp_throttle;
// BITMAP *bmp_winlose;
// BITMAP *bmp_fadeout;

// datafile(s)
DATAFILE *gfxdata; // graphics (terrain, units, structures)
DATAFILE *gfxinter; // interface graphics
DATAFILE *gfxworld; // world/pieces graphics
DATAFILE *gfxmentat; // mentat graphics

// FONT stuff
ALFONT_FONT *game_font; // arrakeen.fon
ALFONT_FONT *bene_font; // benegesserit font.
ALFONT_FONT *small_font; // small font.

// MP3 STUFF
ALMP3_MP3 *mp3_music; // pointer to mp3 music

// Timers
int frame_count, fps; // fps and such

volatile int allegro_timerSecond = 0;
volatile int allegro_timerGlobal = 0;
volatile int allegro_timerUnits = 0;

bool argumentsGiven(int & argc) {
	return argc > 1;
}

bool isGameFlag(string command) {
	return command.compare("-game") == 0;
}

bool hasValueForFlag( int i, int argc ) {
	return (i + 1) < argc;
}

int handleArguments(int argc, char *argv[], cGame * game) {

	if (!argumentsGiven(argc)) return 0;

	for (int i = 1; i < argc; i++) {
		string flag = argv[i];
		if (isGameFlag(flag)) {
			if (hasValueForFlag(i, argc)) {
				i++;
				game->setGameRules(string(argv[i]));
			}
		}
	}

	return 0;
}

int main(int argc, char **argv) {
	Logger * logger = Logger::getInstance();
	logger->renew();

	logger->logHeader("Dune II - The Maker");
	logger->logCommentLine(""); // white space
	logger->logHeader("Version information");
// 	char msg[255];
// 	sprintf(msg, "Version %s, Compiled at %s , %s", game->getVersion(), __DATE__, __TIME__);
// 	logger->log(LOG_INFO, COMP_VERSION, "Initializing", msg);

// 	if (windowed) {
// 		logger->log(LOG_INFO, COMP_SETUP, "Initializing", "Windowed mode");
// 	} else {
// 		logger->log(LOG_INFO, COMP_SETUP, "Initializing", "Fullscreen mode");
// 	}

	// logger->debug notification
	logger->logHeader("Allegro");

	// ALLEGRO - INIT
	if (allegro_init() != 0) {
		logger->log(LOG_FATAL, COMP_ALLEGRO, "Allegro init", allegro_id, OUTC_FAILED);
		return false;
	}

	logger->log(LOG_INFO, COMP_ALLEGRO, "Allegro init", allegro_id, OUTC_SUCCESS);

	int r = install_timer();
	if (r > -1) {
		logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing timer functions", "install_timer()", OUTC_SUCCESS);
	} else {
		allegro_message("Failed to install timer");
		logger->log(LOG_FATAL, COMP_ALLEGRO, "Initializing timer functions", "install_timer()", OUTC_FAILED);
		return false;
	}

	alfont_init();
	logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing ALFONT", "alfont_init()", OUTC_SUCCESS);
	install_keyboard();
	logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing Allegro Keyboard", "install_keyboard()", OUTC_SUCCESS);
	install_mouse();
	Mouse::getInstance();
	logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing Allegro Mouse", "install_mouse()", OUTC_SUCCESS);


	LOCK_VARIABLE(allegro_timerUnits);
	LOCK_VARIABLE(allegro_timerGlobal);
	LOCK_VARIABLE(allegro_timerSecond);

	LOCK_FUNCTION(allegro_timerunits);
	LOCK_FUNCTION(allegro_timerglobal);
	LOCK_FUNCTION(allegro_timerfps);

	// Install timers
	install_int(allegro_timerunits, 100); // 100 miliseconds
	install_int(allegro_timerglobal, 5); // 5 miliseconds
	install_int(allegro_timerfps, 1000); // 1000 miliseconds (seconds)

	logger->log(LOG_INFO, COMP_ALLEGRO, "Set up timer related variables", "LOCK_VARIABLE/LOCK_FUNCTION", OUTC_SUCCESS);

/*	frame_count = fps = 0;*/

// 	set window title
// 	char title[128];
// 	sprintf(title, "Dune II - The Maker [%s] - (by Stefan Hendriks)", game->getVersion());
// 	set_window_title(title);
// 	logger->log(LOG_INFO, COMP_ALLEGRO, "Set up window title", title, OUTC_SUCCESS);

	set_window_title("Dune II - The Maker - By Stefan Hendriks");
	set_color_depth(16);

	ScreenResolution * screenResolution = new ScreenResolution(800, 600);


// 	if (windowed) {
		logger->log(LOG_INFO, COMP_ALLEGRO, "Windowed mode requested.", "Searching for optimal graphics settings");
		int iDepth = desktop_color_depth();

		if (iDepth > 15 && iDepth != 24) {
			char msg[255];
			sprintf(msg, "Desktop color dept is %d.", iDepth);
			logger->log(LOG_INFO, COMP_ALLEGRO, "Analyzing desktop color depth.", msg);
			set_color_depth(iDepth); // run in the same bit depth as the desktop
		} else {
			logger->log(LOG_INFO, COMP_ALLEGRO, "Analyzing desktop color depth.",
					"Could not find color depth, or unsupported color depth found. Will use 16 bit");
			set_color_depth(16);
		}

		//GFX_AUTODETECT_WINDOWED
		r = 0;
#ifdef UNIX
		r = set_gfx_mode(GFX_AUTODETECT_WINDOWED, getScreenResolution()->getWidth(), getScreenResolution()->getHeight(), getScreenResolution()->getWidth(), getScreenResolution()->getHeight());
#else
		r = set_gfx_mode(GFX_DIRECTX_WIN, screenResolution->getWidth(), screenResolution->getHeight(), screenResolution->getWidth(), screenResolution->getHeight());
#endif

		char msg[255];
		sprintf(msg, "Initializing graphics mode (windowed) with resolution %d by %d.", screenResolution->getWidth(), screenResolution->getHeight());

		if (r > -1) {
			logger->log(LOG_INFO, COMP_ALLEGRO, msg, "Successfully created window with graphics mode.", OUTC_SUCCESS);
		} else {
			logger->log(LOG_INFO, COMP_ALLEGRO, msg, "Failed to create window with graphics mode. Fallback to fullscreen.", OUTC_FAILED);

			set_color_depth(16);

			// GFX_DIRECTX_ACCEL / GFX_AUTODETECT
#ifdef UNIX
			r = set_gfx_mode(GFX_XWINDOWS, getScreenResolution()->getWidth(), getScreenResolution()->getHeight(), getScreenResolution()->getWidth(), getScreenResolution()->getHeight());
#else
			r = set_gfx_mode(GFX_DIRECTX_ACCEL, screenResolution->getWidth(), screenResolution->getHeight(), screenResolution->getWidth(), screenResolution->getHeight());
#endif

			if (r > -1) {
				logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing graphics mode (fallback, fullscreen)", "Fallback succeeded.", OUTC_SUCCESS);
				/*windowed = false;*/
				return false;
			} else {
				logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing graphics mode (fallback, fullscreen)", "Fallback failed!", OUTC_FAILED);
				allegro_message("Fatal error:\n\nCould not start game.\n\nGraphics mode (windowed mode & fallback) could not be initialized.");
				return false;
			}
		}
// 	} else {
// 
// 		//bool shouldFindBestScreenResolution = true;
// 		//if (isResolutionInGameINIFoundAndSet()) {
// 		//	logger->debug("Resolution is set in INI file.");
// 
// 		//	setScreenResolutionFromGameIniSettings();
// 
// 		//	r = set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, getScreenResolution()->getWidth(), getScreenResolution()->getHeight(), getScreenResolution()->getWidth(),
// 		//			getScreenResolution()->getHeight());
// 		//	char msg[255];
// 
// 		//	sprintf(msg, "Setting up %dx%d resolution from ini file.", getScreenResolution()->getWidth(), getScreenResolution()->getHeight());
// 
// 		//	logger->log(LOG_INFO, COMP_ALLEGRO, "Custom resolution from ini file.", msg);
// 
// 		//	if ((r > -1)) { // on success
// 		//		shouldFindBestScreenResolution = false;
// 		//	}
// 		//}
// 
// 		// find best possible resolution
// 		if (true) {
// 			BestScreenResolutionFinder bestScreenResolutionFinder;
// 			bestScreenResolutionFinder.checkResolutions();
// 			ScreenResolution * aquiredScreenResolution = bestScreenResolutionFinder.aquireBestScreenResolutionFullScreen();
// 			if (aquiredScreenResolution) {
// 				screenResolution = aquiredScreenResolution;
// 			}
// 		}
// 	}

	alfont_text_mode(-1);
	logger->log(LOG_INFO, COMP_ALLEGRO, "Font settings", "Set mode to -1", OUTC_SUCCESS);

// 	game_font = alfont_load_font("data/arakeen.fon");
// 
// 	if (game_font != NULL) {
// 		logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "loaded arakeen.fon", OUTC_SUCCESS);
// 		alfont_set_font_size(game_font, GAME_FONTSIZE); // set size
// 	} else {
// 		logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "failed to load arakeen.fon", OUTC_FAILED);
// 		allegro_message("Fatal error:\n\nCould not start game.\n\nFailed to load arakeen.fon");
// 		return false;
// 	}
// 
// 	bene_font = alfont_load_font("data/benegess.fon");
// 
// 	if (bene_font != NULL) {
// 		logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "loaded benegess.fon", OUTC_SUCCESS);
// 		alfont_set_font_size(bene_font, 10); // set size
// 	} else {
// 		logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "failed to load benegess.fon", OUTC_FAILED);
// 		allegro_message("Fatal error:\n\nCould not start game.\n\nFailed to load benegess.fon");
// 		return false;
// 	}
// 
// 	small_font = alfont_load_font("data/small.ttf");
// 
// 	if (small_font != NULL) {
// 		logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "loaded small.ttf", OUTC_SUCCESS);
// 		alfont_set_font_size(small_font, 10); // set size
// 	} else {
// 		logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "failed to load small.ttf", OUTC_FAILED);
// 		allegro_message("Fatal error:\n\nCould not start game.\n\nFailed to load small.ttf");
// 		return false;
// 	}

	if (set_display_switch_mode(SWITCH_BACKGROUND) < 0) {
		set_display_switch_mode(SWITCH_PAUSE);
		logger->debug("Display 'switch and pause' mode set");
	} else {
		logger->debug("Display 'switch to background' mode set");
	}

// 	int maxSounds = getAmountReservedVoicesAndInstallSound();
// 	memset(msg, 0, sizeof(msg));
// 
// 	if (maxSounds > -1) {
// 		sprintf(msg, "Successful installed sound. %d voices reserved", maxSounds);
// 		logger->log(LOG_INFO, COMP_SOUND, "Initialization", msg, OUTC_SUCCESS);
// 	} else {
// 		logger->log(LOG_INFO, COMP_SOUND, "Initialization", "Failed installing sound.", OUTC_FAILED);
// 	}

	/***
	 Bitmap Creation
	 ***/

	int width = screenResolution->getWidth();
	int height = screenResolution->getHeight();

	memset(msg, 0, sizeof(msg));
	sprintf(msg, "Creating bitmaps with resolution of %dx%d.", width, height);
	logger->debug(msg);


	/*** End of Bitmap Creation ***/
	set_color_conversion(COLORCONV_MOST);

	logger->debug("Color conversion method set");

	// setup mouse speed
	set_mouse_speed(-1, -1);

	logger->debug("MOUSE: Mouse speed set");

	logger->debug("\n----");
	logger->debug("GAME ");
	logger->debug("----");

	/*** Data files ***/

	// load datafiles
	gfxdata = load_datafile("data/gfxdata.dat");
	if (gfxdata == NULL) {
		logger->debug("ERROR: Could not hook/load datafile: gfxdata.dat");
		return false;
	} else {
		logger->debug("Datafile hooked: gfxdata.dat");
		memcpy(general_palette, gfxdata[PALETTE_D2TM].dat, sizeof general_palette);
	}

	DATAFILE * gfxaudio = load_datafile("data/gfxaudio.dat");
	if (gfxaudio == NULL) {
		logger->debug("ERROR: Could not hook/load datafile: gfxaudio.dat");
		return false;
	} else {
		logger->debug("Datafile hooked: gfxaudio.dat");
	}

// 	soundPlayer = new cSoundPlayer(maxSounds, 255, 150);
// 	soundPlayer->setDatafile(gfxaudio);

	gfxinter = load_datafile("data/gfxinter.dat");
	if (gfxinter == NULL) {
		logger->debug("ERROR: Could not hook/load datafile: gfxinter.dat");
		return false;
	} else {
		logger->debug("Datafile hooked: gfxinter.dat");
	}

	gfxworld = load_datafile("data/gfxworld.dat");
	if (gfxworld == NULL) {
		logger->debug("ERROR: Could not hook/load datafile: gfxworld.dat");
		return false;
	} else {
		logger->debug("Datafile hooked: gfxworld.dat");
	}

	gfxmentat = load_datafile("data/gfxmentat.dat");
	if (gfxworld == NULL) {
		logger->debug("ERROR: Could not hook/load datafile: gfxmentat.dat");
		return false;
	} else {
		logger->debug("Datafile hooked: gfxmentat.dat");
	}

	// randomize timer
	unsigned int t = (unsigned int) time(0);
	char seedtxt[80];
	sprintf(seedtxt, "Seed is %d", t);
	logger->debug(seedtxt);
	srand(t);

	// Mentat class pointer set at null

	set_palette(general_palette);

	// normal sounds are loud, the music is lower (its background music, so it should not be disturbing)
	set_volume(255, 150);

	// A few messages for the player
// 	logger->debug("Installing:  PLAYERS");
// 	INSTALL_PLAYERS();
// 	logger->debug("Installing:  HOUSES");
// 	INSTALL_HOUSES();
// 	logger->debug("Installing:  STRUCTURES");
// 	install_structures();
// 	logger->debug("Installing:  BULLET TYPES");
// 	install_bullets();
// 	logger->debug("Installing:  UNITS");
// 	install_units();
// 	logger->debug("Installing:  WORLD");
// 	INSTALL_WORLD();


	bmp_screen = create_bitmap(width, height);

	if (bmp_screen == NULL) {
		allegro_message("Failed to create a memory bitmap");
		logger->debug("ERROR: Could not create bitmap: bmp_screen");
		return false;
	} else {
		logger->debug("Memory bitmap created: bmp_screen");
		clear(bmp_screen);
	}

	
	Screen * screen = new Screen(screenResolution, bmp_screen);
	State * state = new MainMenuState(screen, Mouse::getInstance());
	cGame * game = new cGame(state);

	if (handleArguments(argc, argv, game) > 0) {
		return 0;
	}


	if (game->setupGame()) {
		game->run();
	}

	game->shutdown();
	delete game;

	return 0;
}
END_OF_MAIN();

