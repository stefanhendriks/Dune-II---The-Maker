/* AllegroFont - a wrapper for FreeType 2 */
/* to render TTF and other font formats with Allegro */

            
/* FreeType 2 is copyright (c) 1996-2000 */
/* David Turner, Robert Wilhelm, and Werner Lemberg */
/* AllegroFont is copyright (c) 2001, 2002 Javier Gonz lez */

/* See FTL.txt (FreeType Project License) for license */


/* example1.c - Shows how to print some text using a given font file */


/* headers */
#include <allegro.h>
#include <alfont.h>


/* defines */
#define SAMPLE_TEXT     "AllegroFont text normal"
#define SAMPLE_TEXT_AA  "AllegroFont text antialiased"


/* show_help - shows the help */
void show_help(void) {
  allegro_message("Use: example1 fontFilename");
}

/* parse_parameters - checks all the parameters are right */
/*                    returns 0 on success or anything else on error */
int parse_parameters(int argc, char *argv[]) {
  /* if we don't have two parameters */
  /* (first parameter is the executable name) always passed, second
     should be the font name */
  if (argc != 2) {
    /* show help and return error */
    show_help();
    return 1;
  }

  /* all ok */
  return 0;
}

/* main */
int main(int argc, char *argv[]) {
  ALFONT_FONT *user_font;

  /* init allegro */
  allegro_init();

  /* set the window title */
  set_window_title("AllegroFont - example1");
  
  /* parse the parameters */
  if (parse_parameters(argc, argv) != 0) {
    /* error while parsing the parameters */
    allegro_exit();
    return 1;
  }

  /* init the gfx mode (try all possible color depths) */
  set_color_depth(16);
  if (set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0) != 0) {
    set_color_depth(15);
    if (set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0) != 0) {
      set_color_depth(32);
      if (set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0) != 0) {
        set_color_depth(24);
        if (set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0) != 0) {
          /* impossible to set the gfx */
          allegro_message("Impossible to set up the gfx mode");
          allegro_exit();
          return 1;
        }
      }
    }
  }

  /* install the keyboard routines */
  install_keyboard();

  /* now init AllegroFont */
  if (alfont_init() != ALFONT_OK) {
    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
    allegro_message("Error while initializing alfont");
    allegro_exit();
    return 1;
  }

  /* try to load the font given by the parameter*/
  user_font = alfont_load_font(argv[1]);
  if (user_font == NULL) {
    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
    allegro_message("Cannot load the font");
    alfont_exit();
    allegro_exit();
    return 1;
  }

  /* change the font height to 40 pixels (this change will only apply if
     it is a scalable font or is a fixed font with that height available) */
  alfont_set_font_size(user_font, 40);

  /* output in the middle of the screen, upper part, without antialiasing */
  alfont_textout_centre(screen, user_font, SAMPLE_TEXT, SCREEN_W / 2, (SCREEN_H / 2) - 100, makecol(255, 255, 255));

  /* output in the middle of the screen, lower part, with antialiasing */
  alfont_textout_centre_aa(screen, user_font, SAMPLE_TEXT_AA, SCREEN_W / 2, SCREEN_H / 2, makecol(255, 255, 255));

  /* read a key */
  readkey();

  /* destroy the font */
  alfont_destroy_font(user_font);

  /* end of the program */
  alfont_exit();
  allegro_exit();
  return 0;
}
END_OF_MAIN();
