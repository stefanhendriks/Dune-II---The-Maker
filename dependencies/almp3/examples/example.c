/* Example program for AllegroMP3, by Peter Wang */


#include <string.h>
#include <allegro.h>
#include "almp3.h"


void putstr(char *s);


#define DATASZ  (1<<15) /* (32768) amount of data to read from disk each time */
#define BUFSZ   (1<<16) /* (65536) size of audiostream buffer */


typedef struct {
  PACKFILE *f;
  ALMP3_MP3STREAM *s;
} MP3FILE;


MP3FILE *open_mp3_file(char *filename) {
  MP3FILE *p = NULL;
  PACKFILE *f = NULL;
  ALMP3_MP3STREAM *s = NULL;
  char data[DATASZ];
  int len;

  if (!(p = (MP3FILE *)malloc(sizeof(MP3FILE))))
    goto error;
  if (!(f = pack_fopen(filename, F_READ)))
    goto error;
  if ((len = pack_fread(data, DATASZ, f)) <= 0)
    goto error;
  if (len < DATASZ) {
    if (!(s = almp3_create_mp3stream(data, len, TRUE)))
      goto error;
  }
  else {
    if (!(s = almp3_create_mp3stream(data, DATASZ, FALSE)))
      goto error;
  }
  p->f = f;
  p->s = s;
  return p;

  error:
  pack_fclose(f);
  free(p);
  return NULL;
}


int play_mp3_file(MP3FILE *mp3, int buflen, int vol, int pan) {
  return almp3_play_mp3stream(mp3->s, buflen, vol, pan);
}


void close_mp3_file(MP3FILE *mp3) {
  if (mp3) {
    pack_fclose(mp3->f);
    almp3_destroy_mp3stream(mp3->s);
    free(mp3);
  }
}


int poll_mp3_file(MP3FILE *mp3) {
  char *data;
  long len;

  data = (char *)almp3_get_mp3stream_buffer(mp3->s);
  if (data) {
    len = pack_fread(data, DATASZ, mp3->f);
    if (len < DATASZ)
      almp3_free_mp3stream_buffer(mp3->s, len);
    else
      almp3_free_mp3stream_buffer(mp3->s, -1);
  }

  return almp3_poll_mp3stream(mp3->s);
}


void do_example(int n, char *filenames[]) {
  MP3FILE **mp3 = (MP3FILE **)malloc(sizeof(MP3FILE *) * n);
  int remain, i;

  remain = n;
  for (i = 0; i < n; i++) {
    putstr(filenames[i]);
    if ((mp3[i] = open_mp3_file(filenames[i])))
      play_mp3_file(mp3[i], BUFSZ, 255, 128);
    else {
      remain--;
      putstr("Error opening.");
    }
  }

  while ((!keypressed()) && (remain > 0)) {
    for (i = 0; i < n; i++) {
      if ((mp3[i]) && (poll_mp3_file(mp3[i]) != ALMP3_OK)) {
        close_mp3_file(mp3[i]);
        mp3[i] = NULL;
        remain--;
      }
    }
  }

  for (i = 0; i < n; i++)
    close_mp3_file(mp3[i]);

  free(mp3);
}


int main(int argc, char *argv[]) { 
  allegro_init();

  if (argc < 2) {
    allegro_message("Usage: %s files ...\n", argv[0]);
    return 1;
  }

  install_timer();
  install_keyboard();

  if (set_gfx_mode(GFX_SAFE, 640, 480, 0, 0) < 0) {
    allegro_message("Error setting video mode.\n");
    return 1;
  }
  clear_to_color(screen, makecol(255, 255, 255));

  /* to achieve the max possible volume */
  set_volume_per_voice(0);

  if (install_sound(DIGI_AUTODETECT, MIDI_NONE, 0) < 0) {
    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
    allegro_message("Error installing sound.\n");
    return 1;
  }

  do_example(argc-1, argv+1);

  allegro_exit();

  return 0;
}
END_OF_MAIN();


/* a hack */
void putstr(char *s) {
  static int y = 0;
  text_mode(-1);
  textout(screen, font, s, 0, y, makecol(0, 0, 0));
  y += text_height(font);
}
