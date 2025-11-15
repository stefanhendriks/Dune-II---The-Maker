#pragma once

class cMapCamera;

class cScreenShake {
public:
    cScreenShake();

    void shake(int duration);
    void reduce();
    void reset();
    int getX() const;
    int getY() const;
    int getTimer() const;
    void update(int state, int playingState, cMapCamera *mapCamera);

private:
    int m_shakeX;
    int m_shakeY;
    int m_TIMER_shake;
};