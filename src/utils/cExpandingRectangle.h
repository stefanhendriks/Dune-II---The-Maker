#pragma once

#include "cRectangle.h"

class cExpandingRectangle : public cRectangle {
public:
    cExpandingRectangle();

    void expand(float x, float y);
    void expandBy(float amount);

    bool isInitialized() const { return m_initialized; }

    int getWidth() const override;
    int getHeight() const override;

private:
    float m_minX, m_maxX, m_minY, m_maxY;
    bool m_initialized;

    void sync();
};
