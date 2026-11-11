/**
 * @file AudioContext.cpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#include "AudioContext.hpp"

#include "context/MusicJukeBox.hpp"
#include "game/cGameSettings.h"
#include "utils/cSoundPlayer.h"
#include "utils/Log.h"
#include "include/definitions.h"
#include "data/SoundPools.h"
#include "data/Sounds.h"
#include "include/cAssert.h"

#include <format>

namespace {
const char* musicTypeName(eMusicType type)
{
    switch (type) {
    case eMusicType::WIN: return "WIN";
    case eMusicType::LOSE: return "LOSE";
    case eMusicType::ATTACK: return "ATTACK";
    case eMusicType::PEACE: return "PEACE";
    case eMusicType::MENU: return "MENU";
    case eMusicType::CONQUEST: return "CONQUEST";
    case eMusicType::BRIEFING: return "BRIEFING";
    }
    return "UNKNOWN";
}
} // namespace

AudioContext::AudioContext(std::unique_ptr<cSoundPlayer> soundPlayer, cGameSettings* settings) :
    m_soundPlayer(std::move(soundPlayer)),
    m_settings(settings),
    m_jukeBox(std::make_unique<MusicJukeBox>()),
    m_newMusic{-1, std::nullopt},
    m_newMusicCountdown(0)
{
    d2tm_assert(m_soundPlayer != nullptr);
    d2tm_assert(m_settings != nullptr);
    m_jukeBox->createRepertoryDirectories();
    const auto lookup = [this](std::string_view name) {
        const int index = m_soundPlayer->getIndexFromName(std::string(name));
        if (index < 0) {
            Logger::error(COMP_SOUND, "AudioContext", "sound '{}' not found in the audio pack", name);
        }
        return index;
    };
    const int missing = soundpools::resolve(lookup) + sounds::resolve(lookup);
    if (missing > 0) {
        Logger::error(COMP_SOUND, "AudioContext", "{} sound pool names not found in the audio pack", missing);
    }
    m_newMusic.archiveTrack = sounds::menuMusic; // only known once resolved
}

AudioContext::~AudioContext() = default;

cSoundPlayer* AudioContext::getSoundPlayer() const
{
    return m_soundPlayer.get();
}

void AudioContext::playSound(int sampleId) const
{
    m_soundPlayer->playSound(sampleId);
}

void AudioContext::playSound(int sampleId, int vol) const
{
    m_soundPlayer->playSound(sampleId, vol);
}

void AudioContext::playVoice(const SoundPool &voice, int house) const
{
    int variant = 0;
    if (house == HARKONNEN) {
        variant = 1;
    }
    else if (house == ORDOS) {
        variant = 2;
    }
    m_soundPlayer->playVoice(voice.at(variant));
}

int AudioContext::getMaxVolume() const
{
    return m_soundPlayer->getMaxVolume();
}

void AudioContext::playMusicByTypeForStateTransition(eMusicType iType, int humanHouse)
{
    if (m_settings->getMusicType() != iType) {
        m_newMusicCountdown = 0;
        playMusicByType(iType, false, false, humanHouse);
    }
}

bool AudioContext::playMusicByType(eMusicType iType, bool triggerWithVoice, bool isCurrentlyPlayingMission, int humanHouse)
{
    Logger::info(COMP_SOUND, "AudioContext::playMusicByType", "iType = {}. triggerWithVoice = {}", musicTypeName(iType), triggerWithVoice);

    if (triggerWithVoice) {
        if (iType == m_settings->getMusicType()) {
            Logger::info(COMP_SOUND, "AudioContext::playMusicByType", "m_musicType = {}, iType is {}, so bailing", musicTypeName(*m_settings->getMusicType()), musicTypeName(iType));
            return false;
        }
    }

    m_settings->setMusicType(iType);
    Logger::info(COMP_SOUND, "AudioContext::playMusicByType", "m_musicType = {}", musicTypeName(iType));

    if (!m_settings->isPlayMusic()) {
        return false; // todo: have a 'no-sound soundplayer' instead of doing this :/
    }

    if (m_newMusicCountdown > 0) {
        // do not interfere with previous 'change to music' thing?
        return false;
    }

    int sampleId = sounds::menuMusic;
    if (iType == eMusicType::WIN) {
        sampleId = soundpools::musicWin.pick();
    }
    else if (iType == eMusicType::LOSE) {
        sampleId = soundpools::musicLose.pick();
    }
    else if (iType == eMusicType::ATTACK) {
        sampleId = soundpools::musicAttack.pick();
    }
    else if (iType == eMusicType::PEACE) {
        sampleId = soundpools::musicPeace.pick();
    }
    else if (iType == eMusicType::MENU) {
        sampleId = sounds::menuMusic;
    }
    else if (iType == eMusicType::CONQUEST) {
        sampleId = sounds::scenarioMusic;
    }
    else if (iType == eMusicType::BRIEFING) {
        if (humanHouse == ATREIDES) {
            sampleId = sounds::mentatMusicAtreides;
        }
        else if (humanHouse == HARKONNEN) {
            sampleId = sounds::mentatMusicHarkonnen;
        }
        else if (humanHouse == ORDOS) {
            sampleId = sounds::mentatMusicOrdos;
        }
        else if (humanHouse == SARDAUKAR) {
            sampleId = sounds::mentatMusicHarkonnen; // no @SARDAUKAR srd music, so use harkonnen one
        }
        else {
            d2tm_assert(false && "Undefined house.");
        }
    }
    else {
        d2tm_assert(false && "Undefined music type.");
    }

    if (triggerWithVoice) {
        // voice triggered music (ie "Enemy unit approaching"), so have music stop a bit
        if (isCurrentlyPlayingMission) {
            m_newMusicCountdown = 400; // wait a bit longer
        }
        else {
            m_newMusicCountdown = 0;
        }
        m_soundPlayer->stopMusic();
    }
    else {
        // instant switch
        m_newMusicCountdown = 0;
    }

    m_jukeBox->setCustomMusicEnabled(m_settings->isCustomMusic());
    m_newMusic = m_jukeBox->choose(iType, sampleId, humanHouse);
    return true;
}

void AudioContext::startMusic(const MusicChoice &choice) const
{
    if (choice.file && m_soundPlayer->playMusicFile(choice.file->string())) {
        return;
    }
    m_soundPlayer->playMusic(choice.archiveTrack);
}

void AudioContext::thinkFast()
{
    if (!m_settings->isPlayMusic()) // no music enabled, so no need to think
        return;

    // all this does is repeating music in the same theme.
    if (!m_settings->getMusicType().has_value())
        return;

    if (m_newMusicCountdown > 0) {
        m_newMusicCountdown--;
    }

    if (m_newMusicCountdown == 0) {
        startMusic(m_newMusic);
        m_newMusicCountdown--; // so we don't keep re-starting music
    }

    if (m_newMusicCountdown < 0) {
        if (!m_soundPlayer->getMusicEnabled()) return;
        if (!m_soundPlayer->isMusicPlaying()) {
            eMusicType desiredMusicType = *m_settings->getMusicType();
            if (desiredMusicType == eMusicType::ATTACK) {
                desiredMusicType = eMusicType::PEACE; // set back to peace
            }

            if (desiredMusicType == eMusicType::BRIEFING) {
                // the briefing track depends on the human house, so replay the sample picked earlier
                startMusic(m_newMusic);
                return;
            }
            playMusicByType(desiredMusicType, false, false, GENERALHOUSE);
        }
    }
}

void AudioContext::toggleMusic()
{
    setMusicEnabled(!m_settings->isPlayMusic());
}

void AudioContext::setMusicEnabled(bool enabled)
{
    m_settings->setPlayMusic(enabled);
    m_soundPlayer->setMusicEnabled(enabled);
    if (enabled) {
        // let thinkFast() start the music of the current music type; nothing may have been picked yet when booted with -nomusic
        m_newMusicCountdown = -1;
    }
}

void AudioContext::changeMusicVolume(int delta) const
{
    m_soundPlayer->changeMusicVolume(delta);
}
