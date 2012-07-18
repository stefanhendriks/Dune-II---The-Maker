/*

 Dune II - The Maker

 Author : Stefan Hendriks
 Contact: stefanhen83@gmail.com
 Website: http://dune2themaker.fundynamic.com

 2001 - 2011 (c) code by Stefan Hendriks

 */

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <string>


#include "domain/Mouse.h"
#include "domain/Viewport.h"
#include "domain/Map.h"

#include "utils/Logger.h"
#include "utils/StringUtils.h"
#include "utils/FileReader.h"

#include "states/MainMenuState.h"
#include "states/PlayingState.h"
#include "Game.h"

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
ALFONT_FONT *game_font = NULL; // arrakeen.fon
ALFONT_FONT *bene_font = NULL; // benegesserit font.
ALFONT_FONT *small_font = NULL; // small font.

// MP3 STUFF
ALMP3_MP3 *mp3_music; // pointer to mp3 music

// Timers
int frame_count, fps; // fps and such

volatile int timerPerSecond = 0;
volatile int timerGameTimer = 0;

bool argumentsGiven(int & argc) {
	return argc > 1;
}

bool isGameFlag(string command) {
	return command.compare("-game") == 0;
}

bool hasValueForFlag( int i, int argc ) {
	return (i + 1) < argc;
}

int handleArguments(int argc, char *argv[], Game * game) {

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

int initTimers(Logger * logger, long gameTimerInMiliseconds, long perSecondTimerInMiliseconds) {
	int r = install_timer();
	if (r > -1) {
		logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing timer functions", "install_timer()", OUTC_SUCCESS);
	} else {
		allegro_message("Failed to install timer");
		logger->log(LOG_FATAL, COMP_ALLEGRO, "Initializing timer functions", "install_timer()", OUTC_FAILED);
		return r;
	}

	LOCK_VARIABLE(timerGameTimer);
	LOCK_VARIABLE(timerPerSecond);

	LOCK_FUNCTION(timerGameTimerFunction);
	LOCK_FUNCTION(timerPerSecondFunction);

	// Install timers
	install_int(timerGameTimerFunction, gameTimerInMiliseconds);
	install_int(timerPerSecondFunction, perSecondTimerInMiliseconds);

	logger->log(LOG_INFO, COMP_ALLEGRO, "Set up timer related variables", "LOCK_VARIABLE/LOCK_FUNCTION", OUTC_SUCCESS);

	return 0;
}

int getIdealColorDepth(Logger * logger) {
	int iDepth = desktop_color_depth();
	char msg[255];
	
	if (iDepth > 15 && iDepth != 24) {
		sprintf(msg, "Desktop color dept is %d.", iDepth);
	} else {
		sprintf(msg, "Could not find color depth, or unsupported color depth found. Will use 16 bit");
		iDepth = 16;
	}
	logger->log(LOG_INFO, COMP_SETUP, "Finding ideal color depth for windowed mode", msg, OUTC_NONE);
	return iDepth;
}

ScreenResolution * initializeGraphicsMode(Logger * logger) {
	ScreenResolution * screenResolution = new ScreenResolution(800, 600);
	set_color_depth(getIdealColorDepth(logger));

	int r = 0;
	r = set_gfx_mode(GFX_DIRECTX_WIN, screenResolution->getWidth(), screenResolution->getHeight(), screenResolution->getWidth(), screenResolution->getHeight());
	char msg[255];
	sprintf(msg, "Initializing graphics mode (windowed) with resolution %d by %d.", screenResolution->getWidth(), screenResolution->getHeight());

	if (r > -1) {
		logger->log(LOG_INFO, COMP_ALLEGRO, msg, "Successfully created window with graphics mode.", OUTC_SUCCESS);
	}
	// TODO: what if this fails?
	return screenResolution;
}

int getAmountReservedVoicesAndInstallSound() {
	int voices = 256;
	while (1) {
		if (voices < 4) {
			// failed!
			return -1;
		}
		reserve_voices(voices, 0);
		char msg[255];
		if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) == 0) {
			sprintf(msg, "Success reserving %d voices.", voices);
			Logger::getInstance()->log(LOG_INFO, COMP_SOUND, "Initialization", msg, OUTC_SUCCESS);
			break;
		} else {
			sprintf(msg, "Failed reserving %d voices. Will try %d.", voices, (voices / 2));
			Logger::getInstance()->log(LOG_INFO, COMP_SOUND, "Initialization", msg, OUTC_FAILED);
			voices /= 2;
		}
	}

	return voices;
}

void randomizeTimer( Logger * logger ) {
	unsigned int t = (unsigned int) time(0);
	char seedtxt[80];
	sprintf(seedtxt, "Seed is %d", t);
	logger->debug(seedtxt);
	srand(t);
}

bool loadFonts( Logger * logger ) 
{
	game_font = alfont_load_font("data/arakeen.fon");

	if (game_font != NULL) {
		logger->log(LOG_INFO, COMP_ALFONT, "Loading font", "loaded arakeen.fon", OUTC_SUCCESS);
		alfont_set_font_size(game_font, 8); // default font size is 8
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
	return true;

}

bool loadDataFiles( Logger * logger ) 
{
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
	return true;
}

bool initAllegro( Logger * logger ) 
{
	if (allegro_init() != 0) {
		logger->log(LOG_FATAL, COMP_ALLEGRO, "Allegro init", allegro_id, OUTC_FAILED);
		return false;
	}
	logger->log(LOG_INFO, COMP_ALLEGRO, "Allegro init", allegro_id, OUTC_SUCCESS);
	return true;
}

void initControls( Logger * logger ) {
	install_keyboard();
	logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing Allegro Keyboard", "install_keyboard()", OUTC_SUCCESS);
	install_mouse();
	logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing Allegro Mouse", "install_mouse()", OUTC_SUCCESS);
}

void initFontLibrary( Logger * logger ) {
	alfont_init();
	logger->log(LOG_INFO, COMP_ALLEGRO, "Initializing ALFONT", "alfont_init()", OUTC_SUCCESS);
	alfont_text_mode(-1);
}

void initDisplaySwitchMode( Logger * logger ) {
	if (set_display_switch_mode(SWITCH_BACKGROUND) < 0) {
		set_display_switch_mode(SWITCH_PAUSE);
		logger->debug("Display 'switch and pause' mode set");
	} else {
		logger->debug("Display 'switch to background' mode set");
	}
}

int getRevisionNumberFromFile() 
{
	FileReader * fileReader = new FileReader("revision.txt");
	int revision = 0;
	if (fileReader->hasNext()) {
		string firstLine = fileReader->getLine();
		revision = StringUtils::getNumberFromString(firstLine);
	}
	delete fileReader;
	return revision;
}


int main(int argc, char **argv) {
	int revision = getRevisionNumberFromFile();
	Version * version = new Version(0,4,6, revision);

	Logger * logger = Logger::getInstance();
	logger->renew();

	logger->logHeader("Dune II - The Maker");
	logger->logCommentLine("");
	logger->logHeader("Version information");
 	logger->log(LOG_INFO, COMP_VERSION, "Initializing", version->asString().c_str());

	logger->logHeader("Allegro");

	if (!initAllegro(logger)) {
		return 1;
	}

	initFontLibrary(logger);
	initControls(logger);
	
	if (initTimers(logger, 5, 1000) < 0) {
		logger->debug("Failed to initialize timers.");
		return 1;
	}

// 	set window title
// 	char title[128];
// 	sprintf(title, "Dune II - The Maker [%s] - (by Stefan Hendriks)", game->getVersion());
// 	set_window_title(title);
// 	logger->log(LOG_INFO, COMP_ALLEGRO, "Set up window title", title, OUTC_SUCCESS);
	set_window_title("Dune II - The Maker - By Stefan Hendriks");
		
	ScreenResolution * screenResolution = initializeGraphicsMode(logger);

	logger->log(LOG_INFO, COMP_ALLEGRO, "Font settings", "Set mode to -1", OUTC_SUCCESS);

	if (!loadFonts(logger)) {
		return 1;
	}
	
	initDisplaySwitchMode(logger);


	int maxSounds = getAmountReservedVoicesAndInstallSound();

	if (maxSounds > -1) {
		char msg[255];
		sprintf(msg, "Successful installed sound. %d voices reserved", maxSounds);
		logger->log(LOG_INFO, COMP_SOUND, "Initialization", msg, OUTC_SUCCESS);
	} else {
		logger->log(LOG_INFO, COMP_SOUND, "Initialization", "Failed installing sound.", OUTC_FAILED);
	}

	/***
	 Bitmap Creation
	 ***/
	set_color_conversion(COLORCONV_MOST);

	int width = screenResolution->getWidth();
	int height = screenResolution->getHeight();
	char msg[255];
	memset(msg, 0, sizeof(msg));
	sprintf(msg, "Creating bitmaps with resolution of %dx%d.", width, height);
	logger->debug(msg);


	/*** End of Bitmap Creation ***/

	logger->debug("Color conversion method set");

	// setup mouse speed
	set_mouse_speed(-1, -1);

	if (!loadDataFiles(logger)) {
		return 1;
	}
	
	randomizeTimer(logger);

	set_palette(general_palette);

	set_volume(255, 150);
	
	bmp_screen = create_bitmap(width, height);

	if (bmp_screen == NULL) {
		allegro_message("Failed to create a memory bitmap");
		logger->debug("ERROR: Could not create bitmap: bmp_screen");
		return false;
	} else {
		logger->debug("Memory bitmap created: bmp_screen");
		clear(bmp_screen);
	}

	

	try {
		Mouse * mouse = new Mouse(new Bitmap((BITMAP *) gfxdata[MOUSE_NORMAL].dat));
		Screen * screen = new Screen(screenResolution, bmp_screen);

		BITMAP * mapBitmap = load_bmp("data\\map.bmp", general_palette);  // TODO: create constructor in Bitmap with filename, that throws CannotFindFileException when result is NULL
		Map * map = new Map(new Bitmap(mapBitmap));

		Viewport * viewPort = new Viewport(300, 300, map);
		
		State * state = new PlayingState(screen, mouse, map, viewPort);
		Game * game = new Game(state, version);

		if (handleArguments(argc, argv, game) > 0) {
			return 0;
		}


		set_trans_blender(0, 0, 0, 128); // reset blending state for allegro
		game->run();

		game->shutdown();

		destroy_bitmap(mapBitmap);

		delete version;
		delete screenResolution;
		delete screen;
		delete mouse;
		delete state;
		delete game;
	} catch (exception &e) {
		logger->debug("Some exception occurred");
		logger->debug(e.what());
		allegro_message(e.what());
	}
	
	// shut down library
	logger->logHeader("SHUTDOWN");
	logger->debug("Starting shutdown");
	/*destroy_bitmap(bmp_fadeout);*/
	destroy_bitmap(bmp_screen);
	/*destroy_bitmap(bmp_throttle);*/
	/*destroy_bitmap(bmp_winlose);*/

	logger->debug("Destroyed bitmaps");
	if (small_font != NULL) {
		logger->debug("Destroyed font - small");
		alfont_destroy_font(small_font);
	}
	if (bene_font != NULL) {
		logger->debug("Destroyed font - bene");
		alfont_destroy_font(bene_font);
	}
	if (game_font != NULL) {
		logger->debug("Destroyed font - game");
		alfont_destroy_font(game_font);
	}
	logger->debug("Destroyed fonts");

	alfont_exit();

	// 	if (mp3_music != NULL) {
	// 		almp3_stop_autopoll_mp3(mp3_music);
	// 		almp3_destroy_mp3(mp3_music);
	// 	}

	// Now we are all neatly closed, we exit Allegro and return to the OS
	allegro_exit();
	logger->debug("Allegro shut down.");
	logger->debug("Thanks for playing.");

	return 0;
}
END_OF_MAIN();

