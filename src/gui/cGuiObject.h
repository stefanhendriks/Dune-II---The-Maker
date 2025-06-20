#pragma once

#include "observers/cInputObserver.h"
#include "utils/cRectangle.h"

class cGuiObject : public cInputObserver {
public:
    virtual ~cGuiObject() = default;

    virtual void draw() const = 0;

protected:
    explicit cGuiObject(const cRectangle &rect) : m_rect(rect) {}
    // any gui object has a position and size. Hence its always a 'rect'.
    cRectangle m_rect;
};
