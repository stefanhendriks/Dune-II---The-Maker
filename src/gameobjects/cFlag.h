#pragma once

#include <utils/cPoint.h>

class cPlayer;

class cFlag {

public:
    cFlag(cPlayer *player, cPoint &absCoords, int frames, int animationDelay);
    ~cFlag() = default;

    void draw();

    void thinkFast();

    static cFlag *createBigFlag(cPlayer *player, cPoint &position);
    static cFlag *createSmallFlag(cPlayer *player, cPoint &position);

    void setBig(bool value) {
        m_big = value;
    }
private:
    cPoint m_absCoords;

    cPlayer *m_player;
    int m_TIMER_animate;

    bool m_big;

    // move to 'type object' later (ie flag definitions)
    int m_animationDelay;
    int m_frames; // max amount of frames

    int m_frame;  // frame to draw

};
