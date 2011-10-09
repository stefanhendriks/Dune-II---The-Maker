/*

 Dune II - The Maker

 Author : Stefan Hendriks
 Contact: stefanhen83@gmail.com
 Website: http://dune2themaker.fundynamic.com

 2001 - 2011 (c) code by Stefan Hendriks

 */

#include <cstring>
#include <string>
#include <iostream>

#include "allegro.h"

void logbook(const char *txt);

void FIX_BORDER_POS(int &x, int &y);
void FIX_POS(int &x, int &y);

bool mouse_pressed_left();
bool mouse_pressed_right();

bool MOUSE_BTN_LEFT();
bool MOUSE_BTN_RIGHT();

// Installment of properties/settings in the game.
void INSTALL_HOUSES();
void INSTALL_PLAYERS();
void INSTALL_WORLD();

void INIT_PREVIEWS();

void install_structures();
void install_units();
void install_bullets();

float health_structure(int i, int w);
float health_bar(float max_w, int i, int w);
float health_unit(int i, float w);

int iFindCloseBorderCell(int iCll);

void mp3_play_file(char filename[255]);

void LOAD_SCENE(std::string scene);

void play_sound_id(int s, int iOnScreen);
bool MIDI_music_playing();
void play_voice(int iType);
void setMusicVolume(int i);

void playMusicByType(int iType);
void Shimmer(int r, int x, int y);
int create_bullet(int type, int cell, int goal_cell, int ownerunit, int ownerstruc);

// Creation of units / structures / bullets / reinforcements, etc
void CREATE_STRUCTURE(int iCell, int iType, int iPlayer, int iPercent);

int iCellOnScreen(int iCell);

void mask_to_color(BITMAP *bmp, int color);
void lit_windtrap_color(BITMAP *bmp, int color);
int getAmountReservedVoicesAndInstallSound();

