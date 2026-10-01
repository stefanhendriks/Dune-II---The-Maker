/**
 * @file AudioContext.hpp
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

#pragma once

#include <memory>

class cSoundPlayer;
class cGameSettings;

class AudioContext {
public:
    AudioContext(std::unique_ptr<cSoundPlayer> soundPlayer, cGameSettings* settings);
    ~AudioContext();

    // Low-level escape hatch for UI that adjusts raw player state directly
    // (volume sliders, instant mute) rather than going through game-level music logic.
    [[nodiscard]] cSoundPlayer* getSoundPlayer() const;

    void playSound(int sampleId) const; // Maximum volume
    void playSound(int sampleId, int vol) const;
    void playVoice(int sampleId, int house) const;
    [[nodiscard]] int getMaxVolume() const;

    void playMusicByTypeForStateTransition(int iType, int humanHouse);
    bool playMusicByType(int iType, int playerId, bool triggerWithVoice, bool isCurrentlyPlayingMission, int humanHouse);

    void thinkFast();

    void toggleMusic();
    void changeMusicVolume(int delta) const;

private:
    std::unique_ptr<cSoundPlayer> m_soundPlayer;
    cGameSettings* m_settings = nullptr;

    int m_newMusicSample;
    int m_newMusicCountdown;
};
