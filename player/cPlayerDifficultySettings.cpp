#include "../include/d2tmh.h"
#include "cPlayerDifficultySettings.h"


cPlayerDifficultySettings::cPlayerDifficultySettings() :
                                m_moveSpeedFactor(1.0f),
                                m_buildSpeedFactor(1.0f),
                                m_harvestSpeedFactor(1.0f),
                                m_dumpSpeedFactor(1.0f),
                                m_inflictDamageFactor(1.0f) {
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

cPlayerDifficultySettings::~cPlayerDifficultySettings() {

}

float cPlayerDifficultySettings::getMoveSpeed(int iUnitType, int slowDown) {
    return (int)(unitInfo[iUnitType].speed) + ((float)slowDown * m_moveSpeedFactor);
}

float cPlayerDifficultySettings::getBuildSpeed(int iSpeed) {
    return (int) (iSpeed * m_buildSpeedFactor);
}

float cPlayerDifficultySettings::getHarvestSpeed(int iSpeed) {
    return (int) (iSpeed * m_harvestSpeedFactor);
}

float cPlayerDifficultySettings::getDumpSpeed(int iSpeed) {
    return (int) (iSpeed * m_dumpSpeedFactor);
}

float cPlayerDifficultySettings::getInflictDamage(int iDamageInflicted) {
    return (int) (iDamageInflicted * m_inflictDamageFactor);
}

cPlayerDifficultySettings *cPlayerDifficultySettings::createFromHouse(int house) {
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
