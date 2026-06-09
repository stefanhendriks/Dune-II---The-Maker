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
#include "game/cGameInterface.h"
#include "gameobjects/projectiles/bullet.h"
#include "gameobjects/map/cMapCamera.h"
#include "gameobjects/map/cMap.h"
#include "sidebar/cSideBar.h"
#include "utils/Log.h"
#include "utils/cSoundPlayer.h"
#include "utils/cIniFile.h"
#include "utils/Graphics.hpp"
#include "utils/InitialGameSettings.hpp"
// #include "gameobjects/particles/cParticles.h"
#include "gameobjects/structures/cStructures.h"
#include "gameobjects/projectiles/cBullets.h"
#include "gameobjects/units/cUnits.h"
#include "utils/d2tm_math.h"
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"
#include "game/cGameSettings.h"
#include <cmath>

#include "data/gfxaudio.h"

static cGameSettings* s_logbookSettings = nullptr;

std::unique_ptr<InitialGameSettings> loadSettingsFromIni(const std::string& filename)
{
    std::shared_ptr<cIniFile> file = std::make_shared<cIniFile>(filename);
    std::unique_ptr<InitialGameSettings> gameSettings = std::make_unique<InitialGameSettings>();

    if (!file->hasSection("SETTINGS")) {
        Logger::error(COMP_GAMERULES, "loadSettingsFromIni", "Expected to find [SETTINGS] in {}", filename);
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

    if (section.hasValue("playMusic"))
        gameSettings->playMusic = section.getBoolean("playMusic");
    if (section.hasValue("playSound"))
        gameSettings->playSound = section.getBoolean("playSound");

    if (section.hasValue("drawUnitDebug"))
        gameSettings->drawUnitDebug = section.getBoolean("drawUnitDebug");
    if (section.hasValue("oneAi"))
        gameSettings->oneAi = section.getBoolean("oneAi");
    if (section.hasValue("disableWormAi"))
        gameSettings->disableWormAi = section.getBoolean("disableWormAi");

    if (section.hasValue("disableReinforcements"))
        gameSettings->disableReinforcements = section.getBoolean("disableReinforcements");
    if (section.hasValue("noAiRest"))
        gameSettings->noAiRest = section.getBoolean("noAiRest");
    if (section.hasValue("drawUsages"))
        gameSettings->drawUsages = section.getBoolean("drawUsages");
    if (section.hasValue("pauseWhenLosingFocus"))
        gameSettings->pauseWhenLosingFocus = section.getBoolean("pauseWhenLosingFocus");

    return gameSettings;
}


void initLogbook(cGameSettings* settings)
{
    s_logbookSettings = settings;
}

void logbook(const std::string &txt)
{
    if (s_logbookSettings && s_logbookSettings->isDebugMode()) {
        Logger::warn(COMP_NONE, "logbook", "{}", txt);
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
float healthBar(float max_w, int i, int w)
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
int iFindCloseBorderCell(int iCll, cGameObjectContext* objects)
{
    return objects->getMap()->findCloseMapBorderCellRelativelyToDestinationCel(iCll);
}


int distanceBetweenCellAndCenterOfScreen(int iCell, cGameObjectContext* objects, cMapCamera* mapCamera)
{
    if (objects->getMapGeometry()->isValidCell(iCell)) {
        int centerX = mapCamera->getViewportCenterX();
        int centerY = mapCamera->getViewportCenterY();

        int cellX = objects->getMapGeometry()->getAbsoluteXPositionFromCell(iCell);
        int cellY = objects->getMapGeometry()->getAbsoluteYPositionFromCell(iCell);

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
int createBullet(int type, int fromCell, int targetCell, int unitWhichShoots, int structureWhichShoots, cGameObjectContext* objects, cInfoContext* infos, cGameInterface* iface, cMapCamera* mapCamera)
{
    int new_id = -1;

    int i = 0;
    for (auto &bullet : objects->getBullets()) {
        if (!bullet.bAlive) {
            new_id = i;
            break;
        }
        i++;
    }

    if (new_id < 0)
        return -1;  // failed

    if (type < 0)
        return -1; // failed


    cBullet &newBullet = objects->getBullets()[new_id];
    newBullet.init();

    newBullet.iType = type;
    newBullet.posX = objects->getMapGeometry()->getAbsoluteXPositionFromCellCentered(fromCell);
    newBullet.posY = objects->getMapGeometry()->getAbsoluteYPositionFromCellCentered(fromCell);
    newBullet.iOwnerStructure = structureWhichShoots;
    newBullet.iOwnerUnit = unitWhichShoots;

    newBullet.targetX = objects->getMapGeometry()->getAbsoluteXPositionFromCellCentered(targetCell);
    newBullet.targetY = objects->getMapGeometry()->getAbsoluteYPositionFromCellCentered(targetCell);

    // if we start firing from a mountain, flag it so the bullet won't be blocked by mountains along
    // the way
    newBullet.bStartedFromMountain = objects->getMap()->getCell(fromCell)->type == TERRAIN_MOUNTAIN;

    int structureIdAtTargetCell = objects->getMap()->getCellIdStructuresLayer(targetCell);
    if (structureIdAtTargetCell > -1) {
        cAbstractStructure *pStructure = objects->getStructures()[structureIdAtTargetCell];
        if (pStructure && pStructure->isValid()) {
            newBullet.targetX = pStructure->getRandomPosX();
            newBullet.targetY = pStructure->getRandomPosY();
        }
    }

    newBullet.bAlive = true;
    newBullet.iFrame = 0;

    newBullet.iPlayer = -1;

    if (unitWhichShoots > -1 ) {
        cUnit *cUnit = objects->getUnit(unitWhichShoots);
        newBullet.iPlayer = cUnit->iPlayer;
        // if an airborn unit shoots (ie Ornithopter), reveal on map for everyone
        if (cUnit->isAirbornUnit()) {
            objects->getMap()->clearShroudForAllPlayers(fromCell, 2);
        }
    }

    if (structureWhichShoots > -1) {
        cAbstractStructure *pStructure = objects->getStructures()[structureWhichShoots];
        newBullet.iPlayer = pStructure->getOwner();

        int unitIdAtTargetCell = objects->getMap()->getCellIdUnitLayer(targetCell);
        if (unitIdAtTargetCell > -1) {
            cUnit *unitTarget = objects->getUnit(unitIdAtTargetCell);
            // reveal for player which is being attacked
            objects->getMap()->clearShroud(fromCell, 2, unitTarget->iPlayer);
        }

        unitIdAtTargetCell = objects->getMap()->getCellIdAirUnitLayer(targetCell);
        if (unitIdAtTargetCell > -1) {
            cUnit *unitTarget = objects->getUnit(unitIdAtTargetCell);
            // reveal for player which is being attacked
            objects->getMap()->clearShroud(fromCell, 2, unitTarget->iPlayer);
        }
    }

    if (newBullet.iPlayer < 0) {
        logbook("New bullet produced without any player!?");
    }

    // play sound (when we have one)
    s_BulletInfo &sBullet = infos->getBulletInfo(type);
    if (sBullet.sound > -1) {
        iface->playSoundWithDistance(sBullet.sound, distanceBetweenCellAndCenterOfScreen(fromCell, objects, mapCamera));
    }

    return new_id;
}

const char *toStringBuildTypeSpecificType(const eBuildType &buildType, const int &specificTypeId, cInfoContext* infos)
{
    if (specificTypeId < 0) {
        return "Unknown";
    }
    switch (buildType) {
        case eBuildType::SPECIAL:
            return infos->getSpecialInfo(specificTypeId).description;
        case eBuildType::UNIT:
            return infos->getUnitInfo(specificTypeId).name;
        case eBuildType::STRUCTURE:
            return infos->getStructureInfo(specificTypeId).name;
        case eBuildType::BULLET:
            return infos->getBulletInfo(specificTypeId).description;
        case eBuildType::UPGRADE:
            return infos->getUpgradeInfo(specificTypeId).description;
        case eBuildType::UNKNOWN:
            return "Unknown";
        default:
            d2tm_assert(false && "Undefined buildType?");
            break;
    }
    return "";
}
