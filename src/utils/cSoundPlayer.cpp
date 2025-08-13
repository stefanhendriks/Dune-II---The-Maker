#include "cSoundPlayer.h"

#include "definitions.h"
#include "utils/cLog.h"

#include "utils/DataPack.hpp"
#include <format>

#include <algorithm>
#include <cassert>
#include <memory>
#include <stdexcept>

#include <SDL2/SDL_mixer.h>
#include <algorithm> // for std::clamp


namespace {

// constexpr int kAllegroMaxNrVoices = 256;
// constexpr int kAllegroMaxVolume = 255;
// constexpr int kAllegroPanCenter = 128;

// constexpr int kFinishedPLaying = -1;
// constexpr int kNoVoiceAvailable = -1;
// constexpr int kNoVoice = -1;
constexpr int kNoLoop = 0;

// constexpr int kMinNrVoices = 4;
// constexpr int kMaxVolume = 220;
constexpr int MinVolume = 0;
constexpr int MaxVolume = MIX_MAX_VOLUME;
constexpr int MaxNbrVoices = 64;
}

class cSoundData {
public:
    cSoundData(const std::string &audiofile)
    {
        auto logger = cLogger::getInstance();

        gfxaudio = std::make_unique<DataPack>(audiofile);
        if (gfxaudio == nullptr) {
            auto msg = std::format("Could not hook/load datafile: {}", audiofile);
            logger->log(LOG_ERROR, COMP_SOUND, "Initialization", msg, OUTC_FAILED);
            throw std::runtime_error(msg);
        }
        else {
            auto msg = std::format("Hooked audiofile: {}", audiofile);
            logger->log(LOG_INFO, COMP_SOUND, "Initialization", msg , OUTC_SUCCESS);
        }
    }

    Mix_Chunk *getSample(int sampleId) const
    {
        return gfxaudio->getSample(sampleId);
    }

    Mix_Music *getMusic(int musicId) const
    {
        return gfxaudio->getMusic(musicId);
    }
private:
    std::unique_ptr<DataPack> gfxaudio;
};

// cSoundPlayer::cSoundPlayer(const cPlatformLayerInit &init) : cSoundPlayer(init, MaxNrVoices)
// {
// }

cSoundPlayer::cSoundPlayer(const std::string &datafile /*const cPlatformLayerInit &*/) //, int maxNrVoices)
    : soundData(std::make_unique<cSoundData>(datafile))
{
    // The platform layer init object is not used here, but since it needs to be passed, it tells
    // the caller that the initialization needs to be performed first.

    auto logger = cLogger::getInstance();

    // if (maxNrVoices < MaxNbrVoices) {
    //     logger->log(LOG_WARN, COMP_SOUND, "Initialization", "Muting all sound.", OUTC_SUCCESS);
    //     return;
    // }

    // int nr_voices = MaxNbrVoices;
    // while (true) {
        // if (nr_voices < kMinNrVoices) {
            // logger->log(LOG_WARN, COMP_SOUND, "Initialization", "Failed installing sound.", OUTC_FAILED);
            // return;
        // }

        if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
            logger->log(LOG_ERROR, COMP_SOUND, "SDL2_mixer initialization", Mix_GetError(), OUTC_FAILED);
            isMusicEnable = false;
            isSoundEnable = false;
        } else {
            logger->log(LOG_INFO, COMP_SOUND, "Initialization", "SDL2_mixer succes", OUTC_SUCCESS);
            isMusicEnable = true;
            isSoundEnable = true;
        }

        int nr_voices = Mix_AllocateChannels(MaxNbrVoices);
        if (nr_voices != MaxNbrVoices) {
            auto msg = std::format("AllocateChannels: {} voices reserved, on {} required", nr_voices,MaxNbrVoices);
            logger->log(LOG_INFO, COMP_SOUND, "Initialization", msg, OUTC_SUCCESS);

            // voices.resize(nr_voices - 1, kNoVoice);
            // break;
        }
        else {
            auto msg = std::format("AllocateChannels: {} voices reserved", nr_voices);
            logger->log(LOG_INFO, COMP_SOUND, "Initialization", msg, OUTC_SUCCESS);
        }
    // }
    musicVolume = MaxVolume/2;
    soundVolume = MaxVolume/2;
    Mix_MasterVolume(soundVolume);
    Mix_VolumeMusic(musicVolume);
}

cSoundPlayer::~cSoundPlayer()
{
}

int cSoundPlayer::getMaxVolume()
{
    // TODO: This will become configurable (so you can set your own max volume for sounds, etc)
    return MaxVolume;
}

void cSoundPlayer::think()
{}

void cSoundPlayer::playSound(int sampleId)
{
    if (!isSoundEnable) {
        return;
    }
    playSound(sampleId, musicVolume);
}

void cSoundPlayer::playSound(int sampleId, int vol)
{
    if (vol <= 0 || !isSoundEnable) {
        return;
    }
   vol = std::clamp(vol, 0, MIX_MAX_VOLUME);

    Mix_Chunk *sample = soundData->getSample(sampleId);
    int tmp = Mix_PlayChannel(-1, sample, 0); // -1 means play on any available channel
    if (tmp<0) {
        cLogger::getInstance()->log(LOG_WARN, COMP_SOUND, "sample ignored", "All channels used");
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
    if (!isMusicEnable) {
        return;
    }
    Mix_PlayMusic(soundData->getMusic(sampleId), kNoLoop);
}

bool cSoundPlayer::isMusicPlaying() const
{
    return Mix_PlayingMusic() > 0;
}

void cSoundPlayer::stopMusic()
{
    Mix_HaltMusic();
}

void cSoundPlayer::setMusicVolume(int vol)
{
    musicVolume = std::clamp(vol, 0, MaxVolume);
    Mix_VolumeMusic(musicVolume);
}

void cSoundPlayer::changeMusicVolume(int delta)
{
    musicVolume = std::clamp(musicVolume + delta, 0, MaxVolume);
    this->setMusicVolume(musicVolume);
}

void cSoundPlayer::setSoundVolume(int vol)
{
    soundVolume = std::clamp(vol, 0, MaxVolume);
    Mix_MasterVolume(musicVolume);
}