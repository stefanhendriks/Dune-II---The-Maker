/**
 * @file cFlag.h
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

#include <utils/cPoint.h>
#include <memory>

class cPlayer;
class cMapCamera;
class cGameSettings;
class SDLDrawer;

class cFlag {

public:
    cFlag(cPlayer *player, cPoint &absCoords, int frames, int animationDelay, cMapCamera *mapCamera, cGameSettings *settings, SDLDrawer *renderer);
    ~cFlag() = default;

    void draw();

    void thinkFast();

    static std::unique_ptr<cFlag> createBigFlag(cPlayer *player, cPoint &position, cMapCamera *mapCamera, cGameSettings *settings, SDLDrawer *renderer);
    static std::unique_ptr<cFlag> createSmallFlag(cPlayer *player, cPoint &position, cMapCamera *mapCamera, cGameSettings *settings, SDLDrawer *renderer);

    void setBig(bool value) {
        m_big = value;
    }
private:
    cPoint m_absCoords;
    cPlayer *m_player = nullptr;
    cMapCamera *m_mapCamera = nullptr;
    cGameSettings *m_settings = nullptr;
    SDLDrawer *m_renderer = nullptr;
    int m_TIMER_animate;

    bool m_big;

    // move to 'type object' later (ie flag definitions)
    int m_animationDelay;
    int m_frames; // max amount of frames

    int m_frame;  // frame to draw

};
