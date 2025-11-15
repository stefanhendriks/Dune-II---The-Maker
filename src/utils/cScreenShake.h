#pragma once

class cMapCamera;

/**
 * @brief Handles screen shaking effect logic (offsets, timer, etc).
 */
class cScreenShake {
public:
    cScreenShake();

    // adds shake duration to the timer.
    void shake(int duration);
    // Reduces the shake timer by one tick (should be called every frame or tick).
    void reduce();
    // Resets the shake effect (timer and offsets).
    void reset();
    // Gets the current X offset for the shake effect.
    int getX() const;
    // Gets the current Y offset for the shake effect.
    int getY() const;
    // Gets the current shake timer value.
    int getTimer() const;
    // Updates the shake offsets based on the current state and camera zoom.
    void update(int state, int playingState, cMapCamera *mapCamera);

private:
    int m_shakeX;
    int m_shakeY;
    int m_timer;
};