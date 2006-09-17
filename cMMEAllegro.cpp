#include "d2tmh.h"

cMMEAllegro::cMMEAllegro() {

}

int cMMEAllegro::getMouseX() {
	return mouse_x;
}

int cMMEAllegro::getMouseY() {
	return mouse_y;
}

int cMMEAllegro::getMouseZ() {
	return mouse_z;
}

bool cMMEAllegro::getMouseButton(int button) {
	if (button == MOUSE_BTN_LEFT) {
		if (mouse_b & 1) {
			return true;
		}
		return false;
	}

	if (button == MOUSE_BTN_RIGHT) {
		if (mouse_b & 2) {
			return true;
		}
		return false;
	}

	return false;
}

/** Allegro draw_sprite */
void cMMEAllegro::drawSprite(MME_SURFACE source, MME_SURFACE dest, int x, int y) {
	draw_sprite(dest, source, x, y);
}

/** Allegro blitting routine **/
void cMMEAllegro::doBlit(MME_SURFACE source, MME_SURFACE dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height) {
	blit(source, dest, source_x, source_y, dest_x, dest_y, width, height);	
}

/** Simple blitting **/
void cMMEAllegro::doBlit(BITMAP *source, BITMAP *dest, int x, int y, int width, int height) {
	doBlit(source, dest, 0, 0, x, y, width, height);	
}

/** Primitive blitting, take source sizes **/
void cMMEAllegro::doBlit(MME_SURFACE source, MME_SURFACE dest, int x, int y) {
	doBlit(source, dest, x, y, source->w, source->h);
}

MME_SURFACE cMMEAllegro::createSurface(int width, int height) {
	return create_bitmap(width, height);	
}

MME_SURFACE cMMEAllegro::createSurface(int width, int height, int bitdepth) {
	if (bitdepth == 8 || bitdepth == 16 || bitdepth == 32) {
		return create_bitmap_ex(bitdepth, width, height); 
	}

	return createSurface(width, height);
}

void cMMEAllegro::colorSurface(MME_SURFACE surface, int color) {
	if (color < 0) {
		clear(surface);
	} else {
		clear_to_color(surface, color);
	}
}

void cMMEAllegro::clearSurface(MME_SURFACE surface) {
	colorSurface(surface, -1);
}

/** 
	Set up the library, install drivers, and do anything needed so anything
	after this function that is called WORKS 

	return false on any trouble
*/
bool cMMEAllegro::setupLib() {
	Logger.print("");
	Logger.printVersion();

	if (game.windowed) {
		Logger.print("Windowed mode");
	} else {
		Logger.print("Fullscreen mode");
	}

	// Logbook notification
	Logger.print("\n-------");
	Logger.print("Allegro");
	Logger.print("-------");

	// ALLEGRO - INIT    
	if (allegro_init() != 0) {
		return false;
	}

	Logger.print(allegro_id);

	yield_timeslice();

	int r = install_timer();
	if (r > -1)  {
		Logger.print("install_timer()");
	} else {
		allegro_message("Failed to install timer");
		Logger.print("FAILED");
		return false;
	}	

	alfont_init();
	Logger.print("alfont_init()");
	install_keyboard();
	Logger.print("install_keyboard()");
	install_mouse();
	Logger.print("install_mouse()");

	Logger.print("setting up timer functions / locking functions & memory");

	/* set up the interrupt routines... */  
	game.TIMER_money=0;
	game.TIMER_message=0;
	game.TIMER_throttle=0;

	LOCK_VARIABLE(allegro_timerUnits);
	LOCK_VARIABLE(allegro_timerGlobal);  
	LOCK_VARIABLE(allegro_timerSecond);

	LOCK_FUNCTION(allegro_timerunits);	
	LOCK_FUNCTION(allegro_timerglobal);  	
	LOCK_FUNCTION(allegro_timerfps);

	// Install timers
	install_int(allegro_timerunits, 100);
	install_int(allegro_timerglobal, 5);  
	install_int(allegro_timerfps, 1000);

	Logger.print("Timers installed");

	// set window title
	char title[128];
	sprintf(title, "Dune II - The Maker [%s] - (by Stefan Hendriks)", game.version);

	// Set window title
	set_window_title(title);  
	char window_title[256];
	sprintf(window_title, "Window title set: [%s]", title);
	Logger.print(window_title);

	set_window_close_button(0);

	set_color_depth(game.BITDEPTH);

	if (game.windowed)	{	
		int r = set_gfx_mode(GFX_AUTODETECT_WINDOWED, game.screen_x, game.screen_y, game.screen_x, game.screen_y);  
		// Failed to set windowed auto-detect mode
		if (r < 0) {
			// GFX_DIRECTX_ACCEL / GFX_AUTODETECT
#ifdef UNIX
			r = set_gfx_mode(GFX_XWINDOWS, game.screen_x, game.screen_y, game.screen_x, game.screen_y);
#else
			r = set_gfx_mode(GFX_DIRECTX_ACCEL, game.screen_x, game.screen_y, game.screen_x, game.screen_y);
#endif
			if (r > -1) {  
				game.windowed = false;
			} else {        
				allegro_message("Failed to set resolution");
				return false;
			}
		}
	} else {
		int r = set_gfx_mode(GFX_AUTODETECT, game.screen_x, game.screen_y, game.screen_x, game.screen_y);

		// succes
		if (r < 0) {
			return false;
		}
	}

	text_mode(-1);
	alfont_text_mode(-1);

	Logger.print("Loading font data");

	// loading font
	game_font = alfont_load_font("data/arakeen.fon");  

	if (game_font != NULL) {
		alfont_set_font_size(game_font, GAME_FONTSIZE); // set size
	} else {
		allegro_message("Failed to load arakeen.fon");
		return false;
	}

	bene_font = alfont_load_font("data/benegess.fon");  

	if (bene_font != NULL) {
		alfont_set_font_size(bene_font, 10); // set size
	} else {
		allegro_message("Failed to load benegess.fon");
		return false;
	}

	if (set_display_switch_mode(SWITCH_BACKGROUND) < 0) {
		set_display_switch_mode(SWITCH_PAUSE);
		Logger.print("Display 'switch and pause' mode set");
	} else {
		Logger.print("Display 'switch to background' mode set");
	}

	// sound
	Logger.print("Initializing sound");
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
			Logger.print(msg);
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
		Logger.print(msg); 
		Logger.print("ERROR: Cannot initialize sound card");
	}

	/***
	Bitmap Creation
	***/

	bmp_screen = create_bitmap(game.screen_x, game.screen_y);

	if (bmp_screen == NULL)
	{
		allegro_message("Failed to create a memory bitmap");
		Logger.print("ERROR: Could not create bitmap: bmp_screen");
		return false;
	}
	else
	{
		Logger.print("Bitmap created: bmp_screen");
		clear(bmp_screen);
	}

	bmp_throttle = create_bitmap(game.screen_x, game.screen_y);

	if (bmp_throttle == NULL)
	{
		allegro_message("Failed to create a memory bitmap");
		Logger.print("ERROR: Could not create bitmap: bmp_throttle");
		return false;
	}
	else {
		Logger.print("Bitmap created: bmp_throttle");	
	}

	bmp_winlose = create_bitmap(game.screen_x, game.screen_y);

	if (bmp_winlose == NULL)
	{
		allegro_message("Failed to create a memory bitmap");
		Logger.print("ERROR: Could not create bitmap: bmp_winlose");
		return false;
	}
	else {
		Logger.print("Bitmap created: bmp_winlose");
	}

	bmp_fadeout = create_bitmap(game.screen_x, game.screen_y);

	if (bmp_fadeout == NULL)
	{
		allegro_message("Failed to create a memory bitmap");
		Logger.print("ERROR: Could not create bitmap: bmp_fadeout");
		return false;
	}
	else {
		Logger.print("Bitmap created: bmp_fadeout");
	} 

	/*** End of Bitmap Creation ***/
	set_color_conversion(COLORCONV_MOST);  

	Logger.print("Color conversion method set");

	// setup mouse speed
	set_mouse_speed(-1,-1);

	Logger.print("MOUSE: Mouse speed set");

	Logger.print("\n----");
	Logger.print("GAME ");
	Logger.print("----");

	/*** Data files ***/

	// load datafiles  
	gfxdata = load_datafile("data/gfxdata.dat");
	if (gfxdata == NULL) {
		Logger.print("ERROR: Could not hook/load datafile: gfxdata.dat");
		return false;
	} else {
		Logger.print("Datafile hooked: gfxdata.dat");
		memcpy (general_palette, gfxdata[PALETTE_D2TM].dat, sizeof general_palette);
	}

	gfxaudio = load_datafile("data/gfxaudio.dat");
	if (gfxaudio == NULL)  {
		Logger.print("ERROR: Could not hook/load datafile: gfxaudio.dat");
		return false;
	} else {
		Logger.print("Datafile hooked: gfxaudio.dat");
	}

	gfxinter = load_datafile("data/gfxinter.dat");
	if (gfxinter == NULL)  {
		Logger.print("ERROR: Could not hook/load datafile: gfxinter.dat");
		return false;
	} else {
		Logger.print("Datafile hooked: gfxinter.dat");
	}

	gfxworld = load_datafile("data/gfxworld.dat");
	if (gfxworld == NULL) {
		Logger.print("ERROR: Could not hook/load datafile: gfxworld.dat");
		return false;
	} else {
		Logger.print("Datafile hooked: gfxworld.dat");
	}

	gfxmentat = load_datafile("data/gfxmentat.dat");
	if (gfxworld == NULL) {
		Logger.print("ERROR: Could not hook/load datafile: gfxmentat.dat");
		return false;
	} else {
		Logger.print("Datafile hooked: gfxmentat.dat");	
	}

	gfxmovie = NULL; // nothing loaded at start. This is done when loading a mission briefing.

	DATAFILE *mp3music = load_datafile("data/mp3mus.dat");

	if (mp3music == NULL) {
		Logger.print("MP3MUS.DAT not found, using MIDI to play music");
		game.bMp3=false;
	} else {
		Logger.print("MP3MUS.DAT found, using mp3 files to play music");
		// Immidiatly load menu music
		game.bMp3=true;
	}

	set_palette(general_palette);  

	// normal sounds are loud, the music is lower (its background music, so it should not be disturbing)
	set_volume(255, 175);

	return true;
}