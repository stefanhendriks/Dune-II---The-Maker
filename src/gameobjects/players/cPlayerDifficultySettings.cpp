/**
 * @file cPlayerDifficultySettings.cpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#include "cPlayerDifficultySettings.h"
#include "context/cInfoContext.h"
#include "definitions.h"

#include <cmath>

cPlayerDifficultySettings::cPlayerDifficultySettings() :
    m_moveSpeedFactor(1.0f),
    m_buildSpeedFactor(1.0f),
    m_harvestSpeedFactor(1.0f),
    m_dumpSpeedFactor(1.0f),
    m_inflictDamageFactor(1.0f)
{
    // generic constructor, with no influence
}
cPlayerDifficultySettings::cPlayerDifficultySettings(float moveSpeedFactor, float buildSpeedFactor,
        float harvestSpeedFactor, float dumpSpeedFactor,
        float inflictDamageFactor) :
    m_moveSpeedFactor(moveSpeedFactor),
    m_buildSpeedFactor(buildSpeedFactor),
    m_harvestSpeedFactor(harvestSpeedFactor),
    m_dumpSpeedFactor(dumpSpeedFactor),
    m_inflictDamageFactor(inflictDamageFactor)
{
    // nothing to do here
}

cPlayerDifficultySettings::~cPlayerDifficultySettings()
{

}

float cPlayerDifficultySettings::getMoveSpeed(int iUnitType, int slowDown)
{
    return m_info->getUnitInfo(iUnitType).speed + slowDown * m_moveSpeedFactor;
}

float cPlayerDifficultySettings::getBuildSpeed(int iSpeed)
{
    return std::floor(iSpeed * m_buildSpeedFactor);
}

float cPlayerDifficultySettings::getHarvestSpeed(int iSpeed)
{
    return std::floor(iSpeed * m_harvestSpeedFactor);
}

float cPlayerDifficultySettings::getDumpSpeed(int iSpeed)
{
    return std::floor(iSpeed * m_dumpSpeedFactor);
}

float cPlayerDifficultySettings::getInflictDamage(int iDamageInflicted)
{
    return std::floor(iDamageInflicted * m_inflictDamageFactor);
}

cPlayerDifficultySettings *cPlayerDifficultySettings::createFromHouse(int house)
{
    switch (house) {
        case ATREIDES:
            return new cPlayerDifficultySettings(); // atreides is boring
        case ORDOS:
            return new cPlayerDifficultySettings(
                       0.8f,
                       0.8f,
                       0.9f,
                       0.9f,
                       0.8f
                   );
        case HARKONNEN:
            return new cPlayerDifficultySettings(
                       1.2f,
                       1.2f,
                       1.1f,
                       1.1f,
                       1.2f
                   );
        case SARDAUKAR:
            return new cPlayerDifficultySettings(
                       1.2f,
                       1.25f, // Sardaukar builds even slower than Harkonnen
                       1.1f,
                       1.1f,
                       1.25f // slightly more damage than Harkonnen
                   );
    }

    return new cPlayerDifficultySettings();
}
