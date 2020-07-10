/* loadpng.h */
/* This file is hereby placed in the public domain. */
#ifndef _included_loadpng_h_
#define _included_loadpng_h_

#ifdef __cplusplus
extern "C" {
#endif



/* Overkill :-) */
#define LOADPNG_VERSION		1
#define LOADPNG_SUBVERSION	3
#define LOADPNG_VERSIONSTR	"1.3"


/* Screen gamma value.  Default is -1, which chooses either:
 *  1) Get environment variable SCREEN_GAMMA
 *  2) Use the default of 2.2
 */
extern double _png_screen_gamma;


/* Choose zlib compression level for saving file.
 * Default is Z_BEST_COMPRESSION.
 */
extern int _png_compression_level;


/* Load a PNG from disk. */
BITMAP *load_png(AL_CONST char *filename, RGB *pal);

/* Load a PNG from memory. */
BITMAP *load_memory_png(AL_CONST void *buffer, int buffer_size, RGB *pal);

/* Save a bitmap to disk in PNG format. */
int save_png(AL_CONST char *filename, BITMAP *bmp, AL_CONST RGB *pal);

/* Adds `PNG' to Allegro's internal file type table.
 * You can then just use load_bitmap and save_bitmap as usual.
 */
void register_png_file_type(void);

/* Register an datafile type ID with Allegro, so that when an object
 * with that type ID is encountered while loading a datafile, that
 * object will be loaded as a PNG file.
 */
void register_png_datafile_object(int id);

/* This is supposed to resemble jpgalleg_init in JPGalleg 2.0, just in
 * case you are lazier than lazy.  It contains these 3 lines of code:
 *  register_png_datafile_object(DAT_ID('P','N','G',' '));
 *  register_png_file_type();
 *  return 0;
 */
int loadpng_init(void);


#ifdef __cplusplus
}
#endif

#endif /* _included_loadpng_h */
