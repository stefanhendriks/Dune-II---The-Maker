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
#include <array>
#include <memory>
#include <SDL3/SDL.h>

struct InitialGameSettings;
class cGameObjectContext;
class cMapCamera;
class cInfoContext;
class cGameInterface;
class cGameSettings;


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
int fastThinkTicksToMs(int ticks);

void initLogbook(cGameSettings* settings);
void logbook(const std::string &txt);

// Color makeColFromString(std::string colorStr);



float healthBar(float max_w, int i, int w);

int iFindCloseBorderCell(int iCll, cGameObjectContext* objects);

int createBullet(int type, int fromCell, int targetCell, int unitWhichShoots, int structureWhichShoots, cGameObjectContext* objects, cInfoContext* infos, cGameInterface* iface, cMapCamera* mapCamera);

int distanceBetweenCellAndCenterOfScreen(int iCell, cGameObjectContext* objects, cMapCamera* mapCamera);

const char *toStringBuildTypeSpecificType(const eBuildType &buildType, const int &specificTypeId, cInfoContext* infos);

std::unique_ptr<InitialGameSettings> loadSettingsFromIni(const std::string& filename);