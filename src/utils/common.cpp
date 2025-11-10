/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#include "common.h"

#include "data/gfxdata.h"
#include "data/gfxinter.h"
#include "d2tmc.h"
#include "gameobjects/projectiles/bullet.h"
#include "map/cMapCamera.h"
#include "sidebar/cSideBar.h"
#include "utils/cLog.h"
#include "utils/cSoundPlayer.h"
#include "utils/cIniFile.h"
#include "utils/Graphics.hpp"
#include "utils/GameSettings.hpp"

#include <cmath>

#include "data/gfxaudio.h"

std::unique_ptr<GameSettings> loadSettingsFromIni(const std::string& filename)
{
    std::shared_ptr<cIniFile> file = std::make_shared<cIniFile>(filename, true);
    std::unique_ptr<GameSettings> gameSettings = std::make_unique<GameSettings>();

    if (!file->hasSection("SETTINGS")) {
        cLogger::getInstance()->log(LOG_ERROR, COMP_GAMERULES, filename, "Expected to find [SETTINGS] in file.ini file");
        return gameSettings;
    }

    const cSection &section = file->getSection("SETTINGS");

    if (section.hasValue("ScreenWidth"))
        gameSettings->screenW = section.getInt("ScreenWidth");
    if (section.hasValue("ScreenHeight"))
        gameSettings->screenH = section.getInt("ScreenHeight");
    if (section.hasValue("CameraDragMoveSpeed"))
        gameSettings->cameraDragMoveSpeed = section.getDouble("CameraDragMoveSpeed");
    if (section.hasValue("CameraBorderOrKeyMoveSpeed"))
        gameSettings->cameraBorderOrKeyMoveSpeed = section.getDouble("CameraBorderOrKeyMoveSpeed");
    if (section.hasValue("CameraEdgeMove"))
        gameSettings->cameraEdgeMove = section.getBoolean("CameraEdgeMove");
    if (section.hasValue("FullScreen"))
        gameSettings->windowed = !section.getBoolean("FullScreen");
    if (section.hasValue("AllowRepeatingReinforcements"))
        gameSettings->allowRepeatingReinforcements = section.getBoolean("AllowRepeatingReinforcements");
    if (section.hasValue("AllTurretsDownOnLowPower"))
        gameSettings->turretsDownOnLowPower = section.getBoolean("AllTurretsDownOnLowPower");
    if (section.hasValue("RocketTurretsDownOnLowPower"))
        gameSettings->rocketTurretsDownOnLowPower = section.getBoolean("RocketTurretsDownOnLowPower");
    if (section.hasValue("GameRules"))
        gameSettings->gameFilename = section.getStringValue("GameRules");
    if (section.hasValue("NoAI"))
        gameSettings->disableAI = section.getBoolean("NoAI");
    if (section.hasValue("Debug"))
        gameSettings->debugMode = section.getBoolean("Debug");
    return gameSettings;
}


/**
 * Default printing in logs. Only will be done if game.isDebugMode() is true.
 * @param txt
 */
void logbook(const std::string &txt)
{
    if (game.isDebugMode()) {
        cLogger *logger = cLogger::getInstance();
        logger->log(LOG_WARN, COMP_NONE, "(logbook)", txt);
    }
}

int slowThinkMsToTicks(int desiredMs)
{
    if (desiredMs < 100) {
        return 1; // fastest thinking is 1 tick (100 ms)
    }
    // "slow" thinking, is 1 tick == 100ms
    return desiredMs / 100;
}

int fastThinkTicksToMs(int ticks)
{
    // "fast" thinking, is 1 tick == 5ms
    return ticks * 5;
}

int fastThinkMsToTicks(int desiredMs)
{
    if (desiredMs < 5) {
        return 1; // fastest thinking is 1 tick (5 ms)
    }
    // "fast" thinking, is 1 tick == 5ms
    return desiredMs / 5;
}

/******************************
 Calculation for SPICE/POWER bars, returns amount of pixels
 ******************************/
// MAX_W = pixels maxed
// I = How much we have (CURRENT STATE)
// W = MAX it can have
float health_bar(float max_w, int i, int w)
{
    float flHP   = i;
    float flMAX  = w;

    if (flHP > flMAX) {
        return max_w;
    }

    // amount of pixels (max_w = 100%)
    auto health = flHP / flMAX;

    return (health * max_w);
}

// return a border cell, close to iCll
int iFindCloseBorderCell(int iCll)
{
    return global_map.findCloseMapBorderCellRelativelyToDestinationCel(iCll);
}


int distanceBetweenCellAndCenterOfScreen(int iCell)
{
    if (global_map.isValidCell(iCell)) {
        int centerX = mapCamera->getViewportCenterX();
        int centerY = mapCamera->getViewportCenterY();

        int cellX = global_map.getAbsoluteXPositionFromCell(iCell);
        int cellY = global_map.getAbsoluteYPositionFromCell(iCell);

        return ABS_length(centerX, centerY, cellX, cellY);
    }
    // return as if very close?
    return 1;
}

/**
 * Creates a bullet, of type, starting at *fromCell* and moving towards *targetCell*. The 'unitWhichShoots' or
 * 'structureWhichShoots' is the owner of the bullet.
 * @param type
 * @param fromCell
 * @param targetCell
 * @param unitWhichShoots
 * @param structureWhichShoots
 * @return
 */
int create_bullet(int type, int fromCell, int targetCell, int unitWhichShoots, int structureWhichShoots)
{
    int new_id = -1;

    for (int i = 0; i < MAX_BULLETS; i++)
        if (bullet[i].bAlive == false) {
            new_id = i;
            break;
        }

    if (new_id < 0)
        return -1;  // failed

    if (type < 0)
        return -1; // failed


    cBullet &newBullet = bullet[new_id];
    newBullet.init();

    newBullet.iType = type;
    newBullet.posX = global_map.getAbsoluteXPositionFromCellCentered(fromCell);
    newBullet.posY = global_map.getAbsoluteYPositionFromCellCentered(fromCell);
    newBullet.iOwnerStructure = structureWhichShoots;
    newBullet.iOwnerUnit = unitWhichShoots;

    newBullet.targetX = global_map.getAbsoluteXPositionFromCellCentered(targetCell);
    newBullet.targetY = global_map.getAbsoluteYPositionFromCellCentered(targetCell);

    // if we start firing from a mountain, flag it so the bullet won't be blocked by mountains along
    // the way
    newBullet.bStartedFromMountain = global_map.getCell(fromCell)->type == TERRAIN_MOUNTAIN;

    int structureIdAtTargetCell = global_map.getCellIdStructuresLayer(targetCell);
    if (structureIdAtTargetCell > -1) {
        cAbstractStructure *pStructure = structure[structureIdAtTargetCell];
        if (pStructure && pStructure->isValid()) {
            newBullet.targetX = pStructure->getRandomPosX();
            newBullet.targetY = pStructure->getRandomPosY();
        }
    }

    newBullet.bAlive = true;
    newBullet.iFrame = 0;

    newBullet.iPlayer = -1;

    if (unitWhichShoots > -1 ) {
        cUnit &cUnit = unit[unitWhichShoots];
        newBullet.iPlayer = cUnit.iPlayer;
        // if an airborn unit shoots (ie Ornithopter), reveal on map for everyone
        if (cUnit.isAirbornUnit()) {
            global_map.clearShroudForAllPlayers(fromCell, 2);
        }
    }

    if (structureWhichShoots > -1) {
        cAbstractStructure *pStructure = structure[structureWhichShoots];
        newBullet.iPlayer = pStructure->getOwner();

        int unitIdAtTargetCell = global_map.getCellIdUnitLayer(targetCell);
        if (unitIdAtTargetCell > -1) {
            cUnit &unitTarget = unit[unitIdAtTargetCell];
            // reveal for player which is being attacked
            global_map.clearShroud(fromCell, 2, unitTarget.iPlayer);
        }

        unitIdAtTargetCell = global_map.getCellIdAirUnitLayer(targetCell);
        if (unitIdAtTargetCell > -1) {
            cUnit &unitTarget = unit[unitIdAtTargetCell];
            // reveal for player which is being attacked
            global_map.clearShroud(fromCell, 2, unitTarget.iPlayer);
        }
    }

    if (newBullet.iPlayer < 0) {
        logbook("New bullet produced without any player!?");
    }

    // play sound (when we have one)
    s_BulletInfo &sBullet = sBulletInfo[type];
    if (sBullet.sound > -1) {
        game.playSoundWithDistance(sBullet.sound, distanceBetweenCellAndCenterOfScreen(fromCell));
    }

    return new_id;
}

const char *toStringBuildTypeSpecificType(const eBuildType &buildType, const int &specificTypeId)
{
    switch (buildType) {
        case eBuildType::SPECIAL:
            return sSpecialInfo[specificTypeId].description;
        case eBuildType::UNIT:
            return sUnitInfo[specificTypeId].name;
        case eBuildType::STRUCTURE:
            return sStructureInfo[specificTypeId].name;
        case eBuildType::BULLET:
            return sBulletInfo[specificTypeId].description;
        case eBuildType::UPGRADE:
            return sUpgradeInfo[specificTypeId].description;
        case eBuildType::UNKNOWN:
            return "Unknown";
        default:
            assert(false && "Undefined buildType?");
            break;
    }
    return "";
}
