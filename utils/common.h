/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2020 (c) code by Stefan Hendriks

  */

void logbook(const char *txt);

bool BORDER_POS(int x, int y);
int BETWEEN(int val, int min, int max);
void FIX_BORDER_POS(int &x, int &y);
void FIX_POS(int &x, int &y);

bool MOUSE_BTN_LEFT();
bool MOUSE_BTN_RIGHT();

int keepBetween(int value, int min, int max);

// Installment of properties/settings in the game.
void INSTALL_HOUSES();
void INSTALL_PLAYERS();

void INIT_PREVIEWS();

void install_structures();
void install_units();
void install_bullets();
void install_upgrades();

float health_bar(float max_w, int i, int w);

int iFindCloseBorderCell(int iCll);

void mp3_play_file(char filename[255]);

void play_sound_id(int s, int volume);
void play_sound_id(int s);
void play_sound_id_with_distance(int s, int iOnScreen);

bool MIDI_music_playing();
void play_voice(int iType);
void setMusicVolume(int i);

void playMusicByType(int iType);
void Shimmer(int r, int x, int y);
int create_bullet(int type, int cell, int goal_cell, int unitWhichShoots, int structureWhichShoots);

int distanceBetweenCellAndCenterOfScreen(int iCell);

void bitmap_replace_color(BITMAP *bmp, int colorToReplace, int newColor);
int getAmountReservedVoicesAndInstallSound();

//void masked_stretch_blit(BITMAP *s, BITMAP *d, int s_x, int s_y, int s_w, int s_h, int d_x, int d_y, int d_w, int d_h);
//void stretch_blit(BITMAP *src, BITMAP *dest, int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int dst_w, int dst_h);
//void stretch_sprite(BITMAP *dst, BITMAP *src, int x, int y, int w, int h);