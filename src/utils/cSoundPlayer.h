#pragma once

#include <memory>
#include <vector>
#include <SDL3_mixer/SDL_mixer.h>

class cSoundData;

class cSoundPlayer {
public:
    // Initialize the platform layer before creating this object.
    explicit cSoundPlayer(const std::string &datafile);
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
        return m_musicVolume*10/128;
    }

    void setSoundVolume(int vol);
    int getSoundVolume() const {
        return m_soundVolume*10/128;
    }
    void changeMusicVolume(int delta);

    void setMusicEnabled(bool mm);
    bool getMusicEnabled() const {
        return m_isMusicEnabled;
    }
    void setSoundEnabled(bool sm);
    bool getSoundEnabled() const {
        return m_isSoundEnabled;
    }

private:
    static constexpr int kSfxTrackPoolSize = 8;

    std::unique_ptr<cSoundData> soundData;
    MIX_Mixer *m_mixer = nullptr;
    MIX_Track *m_musicTrack = nullptr;
    std::vector<MIX_Track *> m_sfxTracks;
    int m_nextSfxTrack = 0;
    int m_musicVolume = 0;
    int m_soundVolume = 0;
    bool m_isMusicEnabled = false;
    bool m_isSoundEnabled = false;
};
