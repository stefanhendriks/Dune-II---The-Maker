#pragma once

#include "cPlatformLayerInit.h"
#include "data/gfxaudio.h" // Use IDs from this file to play samples

#include <memory>
#include <vector>

class cSoundData;

class cSoundPlayer {
public:
    // Initialize the platform layer before creating this object.
    explicit cSoundPlayer(const std::string &datafile /*const cPlatformLayerInit &init*/);
    // cSoundPlayer(const cPlatformLayerInit &init, int maxNrVoices);
    ~cSoundPlayer();

    static int getMaxVolume();

    void playSound(int sampleId); // Maximum volume
    void playSound(int sampleId, int vol);

    // Pass the sample ID of the Atreides voice
    void playVoice(int sampleId, int house);

    void playMusic(int sampleId);
    void stopMusic();
    bool isMusicPlaying() const;

    void setMusicVolume(int vol);
    int getMusicVolume() const {
        return musicVolume;
    }
    void setSoundVolume(int vol);
    int getSoundVolume() const {
        return soundVolume;
    }
    void changeMusicVolume(int delta);
    // think about voices, clear voices, etc.
    void think();

    void setMusicEnable(bool mm) {
        isMusicEnable = mm;
    }
    bool getMusicEnable() const {
        return isMusicEnable;
    }
    void setSoundEnable(bool sm) {
        isSoundEnable = sm;
    }
    bool getSoundEnable() const {
        return isSoundEnable;
    }    

private:
    // std::vector<int> voices;
    std::unique_ptr<cSoundData> soundData;
    int musicVolume;
    int soundVolume;
    bool isMusicEnable;
    bool isSoundEnable;
};
