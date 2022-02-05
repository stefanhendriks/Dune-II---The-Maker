/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */
#pragma once

#include "enums.h"

#include <string>

/**
 * returns ticks for desired amount of miliseconds (for slow thinking, 1 tick == 100ms)
 * @param desiredMs
 * @return
 */
int slowThinkMsToTicks(int desiredMs);

/**
 * returns ticks for desired amount of miliseconds (for fast thinking, 1 tick == 5ms)
 * @param desiredMs
 * @return
 */
int fastThinkMsToTicks(int desiredMs);

void logbook(const std::string& txt);

// Installment of properties/settings in the game.
void INSTALL_HOUSES();

void INIT_PREVIEWS();

void install_structures();
void install_units();
void install_bullets();
void install_upgrades();
void install_specials();
void install_particles();

float health_bar(float max_w, int i, int w);

int iFindCloseBorderCell(int iCll);

void Shimmer(int r, int x, int y);
int create_bullet(int type, int fromCell, int targetCell, int unitWhichShoots, int structureWhichShoots);

int distanceBetweenCellAndCenterOfScreen(int iCell);

const char* toStringBuildTypeSpecificType(const eBuildType &buildType, const int &specificTypeId);
