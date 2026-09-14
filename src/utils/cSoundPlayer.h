/**
 * @file cSoundPlayer.h
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
    static constexpr int kSfxTrackPoolSize = 64;
    static constexpr int kPriorityNormal = 0;
    static constexpr int kPriorityVoice = 1;

    void playSoundWithPriority(int sampleId, int vol, int priority);

    std::unique_ptr<cSoundData> soundData;
    MIX_Mixer *m_mixer = nullptr;
    MIX_Track *m_musicTrack = nullptr;
    std::vector<MIX_Track *> m_sfxTracks;
    std::vector<bool> m_sfxTrackBusy;
    std::vector<int> m_sfxTrackPriority;
    SDL_PropertiesID m_sfxPlayOptions = 0;
    int m_nextSfxTrack = 0;
    int m_musicVolume = 0;
    int m_soundVolume = 0;
    bool m_isMusicEnabled = false;
    bool m_isSoundEnabled = false;
};
