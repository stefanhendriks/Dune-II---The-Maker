#pragma once

#include "cRectangle.h"
#include <cmath>

class ExpandingRectangle : public cRectangle {
public:
    ExpandingRectangle() : m_minX(0), m_maxX(0), m_minY(0), m_maxY(0), m_initialized(false) {}

    void expand(float x, float y) {
        if (!m_initialized) {
            m_minX = m_maxX = x;
            m_minY = m_maxY = y;
            m_initialized = true;
        } else {
            if (x < m_minX) m_minX = x;
            if (x > m_maxX) m_maxX = x;
            if (y < m_minY) m_minY = y;
            if (y > m_maxY) m_maxY = y;
        }
        sync();
    }

    void expandBy(float amount) {
        m_minX -= amount;
        m_maxX += amount;
        m_minY -= amount;
        m_maxY += amount;
        sync();
    }

    bool isInitialized() const { return m_initialized; }

    int getWidth() const override { return static_cast<int>(std::round(m_maxX - m_minX)); }
    int getHeight() const override { return static_cast<int>(std::round(m_maxY - m_minY)); }

private:
    float m_minX, m_maxX, m_minY, m_maxY;
    bool m_initialized;

    void sync() {
        move(static_cast<int>(m_minX), static_cast<int>(m_minY));
        resize(getWidth(), getHeight());
    }
};
