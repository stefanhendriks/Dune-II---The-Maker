#pragma once

#include <memory>
#include <vector>

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
    std::unique_ptr<cSoundData> soundData;
    int m_musicVolume;
    int m_soundVolume;
    bool m_isMusicEnabled;
    bool m_isSoundEnabled;
};
