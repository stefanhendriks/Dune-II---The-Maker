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

    cPlayer *m_player;
    cMapCamera *m_mapCamera;
    cGameSettings *m_settings;
    SDLDrawer *m_renderer = nullptr;
    int m_TIMER_animate;

    bool m_big;

    // move to 'type object' later (ie flag definitions)
    int m_animationDelay;
    int m_frames; // max amount of frames

    int m_frame;  // frame to draw

};
