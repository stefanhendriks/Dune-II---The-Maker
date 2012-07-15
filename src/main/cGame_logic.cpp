/*

 Dune II - The Maker

 Author : Stefan Hendriks
 Contact: stefanhen83@gmail.com
 Website: http://dune2themaker.fundynamic.com

 2001 - 2012 (c) code by Stefan Hendriks

 */

#include <string.h>

#include "include/d2tmh.h"


#include "factory/cGameFactory.h"

#include "movie/cMovieDrawer.h"
#include "drawers/AllegroDrawer.h"

#include "utils/BestScreenResolutionFinder.h"
#include "utils/CellCalculator.h"

#include "states/MainMenuState.h"

#include "utils/FileReader.h"
#include "utils/StringUtils.h"

using namespace std;

cGame::cGame() {
	windowed = true;
	gameStateEnum = MAINMENU;
	mapUtils = NULL;
	map = NULL;
	mapCamera = NULL;
	gameDrawer = NULL;
	state = NULL;
	revision = 0;
	memset(version, 0, sizeof(version));
	sprintf(version, "0.4.6");
}

cGame::~cGame() {
	delete screenResolution;
	screenResolution = NULL;
	delete state;
	state = NULL;
}

void cGame::init() {
	gameStateEnum = MAINMENU;

	FileReader * fileReader = new FileReader("revision.txt");
	if (fileReader->hasNext()) {
		string firstLine = fileReader->getLine();
		revision = StringUtils::getNumberFromString(firstLine);
	}
}

/**
 Main game loop
 */
void cGame::run() {
	set_trans_blender(0, 0, 0, 128);
	while (!state->shouldQuitGame()) {
		state->run();
	}
}

/**
 Shutdown the game
 */
void cGame::shutdown() {
	Logger *logger = Logger::getInstance();
	logger->logHeader("SHUTDOWN");
	logger->debug("Starting shutdown");

	destroy_bitmap(bmp_fadeout);
	destroy_bitmap(bmp_screen);
	destroy_bitmap(bmp_throttle);
	destroy_bitmap(bmp_winlose);
	logger->debug("Destroyed bitmaps");

	// Destroy font of Allegro FONT library
	// this somehow seems to crash!
// 	if (game_font != NULL) alfont_destroy_font(game_font);
// 	if (bene_font != NULL) alfont_destroy_font(bene_font);
	logger->debug("Destroyed fonts");

	// Exit the font library (must be first)
	alfont_exit();
	logbook("Allegro FONT library shut down.");

	if (mp3_music != NULL) {
		almp3_stop_autopoll_mp3(mp3_music);
		almp3_destroy_mp3(mp3_music);
	}

	logbook("Allegro MP3 library shut down.");

	// Now we are all neatly closed, we exit Allegro and return to the OS
	allegro_exit();
	logbook("Allegro shut down.");
	logbook("Thanks for playing.");
}

bool cGame::setupGame() {
	// this will empty the log file (create a new one)
	FILE *fp;
	fp = fopen("log.txt", "wt");

	if (fp) {
		fclose(fp);
	}

	Logger *logger = Logger::getInstance();

	// TODO: remove? something seriously is wrong with the initialization of the game
	game.init(); // Must be first!

	// Each time we run the game, we clear out the logbook

	logger->logHeader("Dune II - The Maker");
	logger->logCommentLine(""); // white space

	logger->logHeader("Version information");
	char msg[255];
	sprintf(msg, "Version %s, Compiled at %s , %s", game.version, __DATE__, __TIME__);
	logger->log(LOG_INFO, COMP_VERSION, "Initializing", msg);

	// init game
	if (game.windowed) {
		logger->log(LOG_INFO, COMP_SETUP, "Initializing", "Windowed mode");
	} else {
		logger->log(LOG_INFO, COMP_SETUP, "Initializing", "Fullscreen mode");
	}

	// Logbook notification
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
	assert(Mouse::getInstance());
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

	frame_count = fps = 0;

	// set window title
	char title[128];
	sprintf(title, "Dune II - The Maker [%s] - (by Stefan Hendriks)", game.version);

	// Set window title
	set_window_title(title);
	logger->log(LOG_INFO, COMP_ALLEGRO, "Set up window title", title, OUTC_SUCCESS);

	set_color_depth(16);

	// TODO: read/write rest value so it does not have to 'fine-tune'
	// but is already set up. Perhaps even offer it in the options screen? So the user
	// can specify how much CPU this game may use?
	ScreenResolution * screenResolution = new ScreenResolution(800, 600);


	if (game.windowed) {
		Logger::getInstance()->log(LOG_INFO, COMP_ALLEGRO, "Windowed mode requested.", "Searching for optimal graphics settings");
		int iDepth = desktop_color_depth();

		if (iDepth > 15 && iDepth != 24) {
			char msg[255];
			sprintf(msg, "Desktop color dept is %d.", iDepth);
			Logger::getInstance()->log(LOG_INFO, COMP_ALLEGRO, "Analyzing desktop color depth.", msg);
			set_color_depth(iDepth); // run in the same bit depth as the desktop
		} else {
			Logger::getInstance()->log(LOG_INFO, COMP_ALLEGRO, "Analyzing desktop color depth.",
					"Could not find color depth, or unsupported color depth found. Will use 16 bit");
			set_color_depth(16);
		}

		//GFX_AUTODETECT_WINDOWED
		int r = 0;
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
				game.windowed = false;
			} else {
				logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing graphics mode (fallback, fullscreen)", "Fallback failed!", OUTC_FAILED);
				allegro_message("Fatal error:\n\nCould not start game.\n\nGraphics mode (windowed mode & fallback) could not be initialized.");
				return false;
			}
		}
	} else {

		//bool shouldFindBestScreenResolution = true;
		//if (isResolutionInGameINIFoundAndSet()) {
		//	logbook("Resolution is set in INI file.");

		//	setScreenResolutionFromGameIniSettings();

		//	r = set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, getScreenResolution()->getWidth(), getScreenResolution()->getHeight(), getScreenResolution()->getWidth(),
		//			getScreenResolution()->getHeight());
		//	char msg[255];

		//	sprintf(msg, "Setting up %dx%d resolution from ini file.", getScreenResolution()->getWidth(), getScreenResolution()->getHeight());

		//	Logger::getInstance()->log(LOG_INFO, COMP_ALLEGRO, "Custom resolution from ini file.", msg);

		//	if ((r > -1)) { // on success
		//		shouldFindBestScreenResolution = false;
		//	}
		//}

		// find best possible resolution
		if (true) {
			BestScreenResolutionFinder bestScreenResolutionFinder;
			bestScreenResolutionFinder.checkResolutions();
			ScreenResolution * aquiredScreenResolution = bestScreenResolutionFinder.aquireBestScreenResolutionFullScreen();
			if (aquiredScreenResolution) {
				screenResolution = aquiredScreenResolution;
			}
		}
	}

	alfont_text_mode(-1);
	logger->log(LOG_INFO, COMP_ALLEGRO, "Font settings", "Set mode to -1", OUTC_SUCCESS);

	game_font = alfont_load_font("data/arakeen.fon");

	if (game_font != NULL) {
		logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "loaded arakeen.fon", OUTC_SUCCESS);
		alfont_set_font_size(game_font, GAME_FONTSIZE); // set size
	} else {
		logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "failed to load arakeen.fon", OUTC_FAILED);
		allegro_message("Fatal error:\n\nCould not start game.\n\nFailed to load arakeen.fon");
		return false;
	}

	bene_font = alfont_load_font("data/benegess.fon");

	if (bene_font != NULL) {
		logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "loaded benegess.fon", OUTC_SUCCESS);
		alfont_set_font_size(bene_font, 10); // set size
	} else {
		logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "failed to load benegess.fon", OUTC_FAILED);
		allegro_message("Fatal error:\n\nCould not start game.\n\nFailed to load benegess.fon");
		return false;
	}

	small_font = alfont_load_font("data/small.ttf");

	if (small_font != NULL) {
		logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "loaded small.ttf", OUTC_SUCCESS);
		alfont_set_font_size(small_font, 10); // set size
	} else {
		logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "failed to load small.ttf", OUTC_FAILED);
		allegro_message("Fatal error:\n\nCould not start game.\n\nFailed to load small.ttf");
		return false;
	}

	if (set_display_switch_mode(SWITCH_BACKGROUND) < 0) {
		set_display_switch_mode(SWITCH_PAUSE);
		logbook("Display 'switch and pause' mode set");
	} else {
		logbook("Display 'switch to background' mode set");
	}

	int maxSounds = getAmountReservedVoicesAndInstallSound();
	memset(msg, 0, sizeof(msg));

	if (maxSounds > -1) {
		sprintf(msg, "Successful installed sound. %d voices reserved", maxSounds);
		logger->log(LOG_INFO, COMP_SOUND, "Initialization", msg, OUTC_SUCCESS);
	} else {
		logger->log(LOG_INFO, COMP_SOUND, "Initialization", "Failed installing sound.", OUTC_FAILED);
	}

	/***
	 Bitmap Creation
	 ***/

	int width = screenResolution->getWidth();
	int height = screenResolution->getHeight();

	memset(msg, 0, sizeof(msg));
	sprintf(msg, "Creating bitmaps with resolution of %dx%d.", width, height);
	logbook(msg);

	bmp_screen = create_bitmap(width, height);

	if (bmp_screen == NULL) {
		allegro_message("Failed to create a memory bitmap");
		logbook("ERROR: Could not create bitmap: bmp_screen");
		return false;
	} else {
		logbook("Memory bitmap created: bmp_screen");
		clear(bmp_screen);
	}

	bmp_throttle = create_bitmap(width, height);

	if (bmp_throttle == NULL) {
		allegro_message("Failed to create a memory bitmap");
		logbook("ERROR: Could not create bitmap: bmp_throttle");
		return false;
	} else {
		logbook("Memory bitmap created: bmp_throttle");
	}

	bmp_winlose = create_bitmap(width, height);

	if (bmp_winlose == NULL) {
		allegro_message("Failed to create a memory bitmap");
		logbook("ERROR: Could not create bitmap: bmp_winlose");
		return false;
	} else {
		logbook("Memory bitmap created: bmp_winlose");
	}

	bmp_fadeout = create_bitmap(width, height);

	if (bmp_fadeout == NULL) {
		allegro_message("Failed to create a memory bitmap");
		logbook("ERROR: Could not create bitmap: bmp_fadeout");
		return false;
	} else {
		logbook("Memory bitmap created: bmp_fadeout");
	}

	/*** End of Bitmap Creation ***/
	set_color_conversion(COLORCONV_MOST);

	logbook("Color conversion method set");

	// setup mouse speed
	set_mouse_speed(-1, -1);

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
		memcpy(general_palette, gfxdata[PALETTE_D2TM].dat, sizeof general_palette);
	}

	DATAFILE * gfxaudio = load_datafile("data/gfxaudio.dat");
	if (gfxaudio == NULL) {
		logbook("ERROR: Could not hook/load datafile: gfxaudio.dat");
		return false;
	} else {
		logbook("Datafile hooked: gfxaudio.dat");
	}

// 	soundPlayer = new cSoundPlayer(maxSounds, 255, 150);
// 	soundPlayer->setDatafile(gfxaudio);

	gfxinter = load_datafile("data/gfxinter.dat");
	if (gfxinter == NULL) {
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

	// randomize timer
	unsigned int t = (unsigned int) time(0);
	char seedtxt[80];
	sprintf(seedtxt, "Seed is %d", t);
	logbook(seedtxt);
	srand(t);

	game.gameStateEnum = INITIAL;

	// Mentat class pointer set at null
	Mentat = NULL;

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

	game.init();
	cGameFactory::getInstance()->createGameControlsContextsForPlayers();
	cGameFactory::getInstance()->createMapClassAndNewDependenciesForGame(MAINMENU);

	playMusicByType(MUSIC_MENU);

	Screen * screen = new Screen(screenResolution, bmp_screen);
	state = new MainMenuState(screen, Mouse::getInstance());

	// all has installed well. Lets rock and role.
	return true;

}