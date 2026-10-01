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

#include "game/cGameSettings.h"
#include "utils/cSoundPlayer.h"
#include "utils/RNG.hpp"
#include "utils/Log.h"
#include "include/definitions.h"
#include "data/gfxaudio.h"
#include "include/cAssert.h"

#include <format>

AudioContext::AudioContext(std::unique_ptr<cSoundPlayer> soundPlayer, cGameSettings* settings) :
    m_soundPlayer(std::move(soundPlayer)),
    m_settings(settings),
    m_newMusicSample(MUSIC_MENU),
    m_newMusicCountdown(0)
{
    d2tm_assert(m_soundPlayer != nullptr);
    d2tm_assert(m_settings != nullptr);
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

void AudioContext::playVoice(int sampleId, int house) const
{
    m_soundPlayer->playVoice(sampleId, house);
}

int AudioContext::getMaxVolume() const
{
    return m_soundPlayer->getMaxVolume();
}

void AudioContext::playMusicByTypeForStateTransition(int iType, int humanHouse)
{
    if (m_settings->getMusicType() != iType) {
        m_newMusicCountdown = 0;
        playMusicByType(iType, HUMAN, false, false, humanHouse);
    }
}

bool AudioContext::playMusicByType(int iType, int playerId, bool triggerWithVoice, bool isCurrentlyPlayingMission, int humanHouse)
{
    if (playerId != HUMAN) {
        // skip music we want to play for non human player
        return false;
    }

    Logger::info(COMP_SOUND, "AudioContext::playMusicByType", "iType = {}. playerId = {}, triggerWithVoice = {}", iType, playerId, triggerWithVoice);

    if (triggerWithVoice) {
        if (iType == m_settings->getMusicType()) {
            Logger::info(COMP_SOUND, "AudioContext::playMusicByType", "m_musicType = {}, iType is {}, so bailing", m_settings->getMusicType(), iType);
            return false;
        }
    }

    m_settings->setMusicType(iType);
    Logger::info(COMP_SOUND, "AudioContext::playMusicByType", "m_musicType = {}", m_settings->getMusicType());

    if (!m_settings->isPlayMusic()) {
        return false; // todo: have a 'no-sound soundplayer' instead of doing this :/
    }

    if (m_newMusicCountdown > 0) {
        // do not interfere with previous 'change to music' thing?
        return false;
    }

    int sampleId = MIDI_MENU;
    if (iType == MUSIC_WIN) {
        sampleId = MIDI_WIN01 + RNG::rnd(3);
    }
    else if (iType == MUSIC_LOSE) {
        sampleId = MIDI_LOSE01 + RNG::rnd(6);
    }
    else if (iType == MUSIC_ATTACK) {
        sampleId = MIDI_ATTACK01 + RNG::rnd(6);
    }
    else if (iType == MUSIC_PEACE) {
        sampleId = MIDI_BUILDING01 + RNG::rnd(9);
    }
    else if (iType == MUSIC_MENU) {
        sampleId = MIDI_MENU;
    }
    else if (iType == MUSIC_CONQUEST) {
        sampleId = MIDI_SCENARIO;
    }
    else if (iType == MUSIC_BRIEFING) {
        if (humanHouse == ATREIDES) {
            sampleId = MIDI_MENTAT_ATR;
        }
        else if (humanHouse == HARKONNEN) {
            sampleId = MIDI_MENTAT_HAR;
        }
        else if (humanHouse == ORDOS) {
            sampleId = MIDI_MENTAT_ORD;
        }
        else if (humanHouse == SARDAUKAR) {
            sampleId = MIDI_MENTAT_HAR; // no @SARDAUKAR srd music, so use harkonnen one
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

    m_newMusicSample = sampleId;
    return true;
}

void AudioContext::thinkFast()
{
    if (!m_settings->isPlayMusic()) // no music enabled, so no need to think
        return;

    // all this does is repeating music in the same theme.
    if (m_settings->getMusicType() < 0)
        return;

    if (m_newMusicCountdown > 0) {
        m_newMusicCountdown--;
    }

    if (m_newMusicCountdown == 0) {
        m_soundPlayer->playMusic(m_newMusicSample);
        m_newMusicCountdown--; // so we don't keep re-starting music
    }

    if (m_newMusicCountdown < 0) {
        if (!m_soundPlayer->getMusicEnabled()) return;
        if (!m_soundPlayer->isMusicPlaying()) {
            int desiredMusicType = m_settings->getMusicType();
            if (desiredMusicType == MUSIC_ATTACK) {
                desiredMusicType = MUSIC_PEACE; // set back to peace
            }
            playMusicByType(desiredMusicType, HUMAN, false, false, GENERALHOUSE);
        }
    }
}

void AudioContext::toggleMusic()
{
    m_settings->setPlayMusic(!m_settings->isPlayMusic());
    if (!m_settings->isPlayMusic()) {
        m_soundPlayer->stopMusic();
    }
    else {
        m_soundPlayer->playMusic(m_newMusicSample);
    }
}

void AudioContext::changeMusicVolume(int delta) const
{
    m_soundPlayer->changeMusicVolume(delta);
}
