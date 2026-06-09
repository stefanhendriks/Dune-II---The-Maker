#include "cSoundPlayer.h"

#include "definitions.h"
#include "utils/Log.h"

#include "utils/cDataPack.hpp"
#include <format>

#include <algorithm>
#include "include/cAssert.h"
#include <memory>
#include <stdexcept>

#include <SDL3_mixer/SDL_mixer.h>

namespace {
    constexpr int kNoLoop = 0;
    constexpr int MaxVolume = 128;
}

class cSoundData {
public:
    cSoundData(const std::string &audiofile, MIX_Mixer *mixer)
    {
        gfxaudio = std::make_unique<DataPack>(audiofile, mixer);
        if (gfxaudio == nullptr) {
            auto msg = std::format("Could not hook/load datafile: {}", audiofile);
            Logger::error(COMP_SOUND, "Initialization", "{}", msg);
            throw std::runtime_error(msg);
        }
        else {
            Logger::info(COMP_SOUND, "Initialization", "Hooked audiofile: {}", audiofile);
        }
    }

    MIX_Audio *getAudio(int id) const
    {
        return gfxaudio->getAudio(id);
    }

private:
    std::unique_ptr<DataPack> gfxaudio;
};

cSoundPlayer::cSoundPlayer(const std::string &datafile)
{
    m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (!m_mixer) {
        Logger::error(COMP_SOUND, "cSoundPlayer", "SDL_mixer initialization: {}", SDL_GetError());
        m_isMusicEnabled = false;
        m_isSoundEnabled = false;
    }
    else {
        Logger::info(COMP_SOUND, "cSoundPlayer", "SDL_mixer succes");
        m_isMusicEnabled = true;
        m_isSoundEnabled = true;
        m_musicTrack = MIX_CreateTrack(m_mixer);
    }

    soundData = std::make_unique<cSoundData>(datafile, m_mixer);

    m_musicVolume = MaxVolume / 2;
    m_soundVolume = MaxVolume / 2;
    if (m_mixer) {
        MIX_SetMixerGain(m_mixer, m_soundVolume / (float)MaxVolume);
    }
    if (m_musicTrack) {
        MIX_SetTrackGain(m_musicTrack, m_musicVolume / (float)MaxVolume);
    }
}

cSoundPlayer::~cSoundPlayer()
{
    if (m_musicTrack) {
        MIX_StopTrack(m_musicTrack, 0);
        MIX_DestroyTrack(m_musicTrack);
    }
    if (m_mixer) {
        MIX_DestroyMixer(m_mixer);
    }
}

int cSoundPlayer::getMaxVolume()
{
    return MaxVolume;
}

void cSoundPlayer::playSound(int sampleId)
{
    if (!m_isSoundEnabled) {
        return;
    }
    playSound(sampleId, m_soundVolume);
}

void cSoundPlayer::playSound(int sampleId, int vol)
{
    if (vol <= 0 || !m_isSoundEnabled || !m_mixer) {
        return;
    }
    MIX_Audio *audio = soundData->getAudio(sampleId);
    if (audio) {
        MIX_PlayAudio(m_mixer, audio);
    }
}

void cSoundPlayer::playVoice(int sampleId, int house)
{
    if (house == HARKONNEN) {
        sampleId += 1;
    }
    else if (house == ORDOS) {
        sampleId += 2;
    }

    playSound(sampleId);
}

void cSoundPlayer::playMusic(int sampleId)
{
    if (!m_isMusicEnabled || !m_musicTrack) {
        return;
    }
    MIX_Audio *audio = soundData->getAudio(sampleId);
    if (!audio) {
        cLogger::getInstance()->log(LOG_ERROR, COMP_SOUND, "playMusic",
            std::format("Audio sample [{}] not found, disabling music.", sampleId));
        m_isMusicEnabled = false;
        return;
    }
    MIX_StopTrack(m_musicTrack, 0);
    MIX_SetTrackAudio(m_musicTrack, audio);
    MIX_SetTrackLoops(m_musicTrack, kNoLoop);
    MIX_PlayTrack(m_musicTrack, 0);
}

bool cSoundPlayer::isMusicPlaying() const
{
    if (!m_musicTrack) {
        return false;
    }
    return MIX_TrackPlaying(m_musicTrack);
}

void cSoundPlayer::stopMusic()
{
    if (m_musicTrack) {
        MIX_StopTrack(m_musicTrack, 0);
    }
}

void cSoundPlayer::setMusicVolume(int _vol)
{
    int vol = _vol * 128 / 10;
    m_musicVolume = std::clamp(vol, 0, MaxVolume);
    if (m_musicTrack) {
        MIX_SetTrackGain(m_musicTrack, m_musicVolume / (float)MaxVolume);
    }
}

void cSoundPlayer::changeMusicVolume(int delta)
{
    m_musicVolume = std::clamp(m_musicVolume + delta, 0, MaxVolume);
    this->setMusicVolume(m_musicVolume);
}

void cSoundPlayer::setSoundVolume(int _vol)
{
    int vol = _vol * 128 / 10;
    m_soundVolume = std::clamp(vol, 0, MaxVolume);
    if (m_mixer) {
        MIX_SetMixerGain(m_mixer, m_soundVolume / (float)MaxVolume);
    }
}

void cSoundPlayer::setSoundEnabled(bool sm)
{
    m_isSoundEnabled = sm;
    if (!m_isSoundEnabled && m_mixer) {
        MIX_StopAllTracks(m_mixer, 0);
    }
}

void cSoundPlayer::setMusicEnabled(bool mm)
{
    m_isMusicEnabled = mm;
    if (!m_isMusicEnabled && m_musicTrack) {
        MIX_StopTrack(m_musicTrack, 0);
    }
}
