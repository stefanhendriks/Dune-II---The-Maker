#include "utils/cScreenShake.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include "map/cMapCamera.h"
#include "utils/RNG.hpp"

cScreenShake::cScreenShake()
    : m_shakeX(0), m_shakeY(0), m_TIMER_shake(0)
{}

void cScreenShake::shake(int duration)
{
    m_TIMER_shake += duration;
}

void cScreenShake::reduce()
{
    if (m_TIMER_shake > 0)
        m_TIMER_shake--;
}

void cScreenShake::reset()
{
    m_shakeX = 0;
    m_shakeY = 0;
    m_TIMER_shake = 0;
}

int cScreenShake::getX() const
{
    return m_shakeX;
}

int cScreenShake::getY() const
{
    return m_shakeY;
}

int cScreenShake::getTimer() const
{
    return m_TIMER_shake;
}

void cScreenShake::update(int state, int playingState, cMapCamera *mapCamera)
{
    if (m_TIMER_shake == 0) {
        m_TIMER_shake = -1;
    }

    if (m_TIMER_shake > 0 && state == playingState) {
        // the more we get to the 'end' the less we 'throttle'.
        // Structure explosions are 6 time units per cell.
        // Max is 9 cells (9*6=54)
        // the max border is then 9. So, we do time / 6
        int shakiness = std::min(m_TIMER_shake, 69);
        float offset = mapCamera->factorZoomLevel(std::min(shakiness / 5, 9));
        m_shakeX = -abs(offset / 2) + RNG::rnd(offset);
        m_shakeY = -abs(offset / 2) + RNG::rnd(offset);
    } 
    // else {
        // m_shakeX = 0;
        // m_shakeY = 0;
        // m_TIMER_shake = -1;
    // }
}