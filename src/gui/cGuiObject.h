#pragma once

#include "observers/cInputObserver.h"
#include "utils/cRectangle.h"
#include "gui/GuiTheme.hpp"

class GuiObject : public cInputObserver {
public:
    virtual ~GuiObject() = default;
    virtual void draw() const = 0;

    void setTheme(const GuiTheme& theme);
protected:
    explicit GuiObject(const cRectangle &rect) : m_rect(rect) {}
    // any gui object has a position and size. Hence its always a 'rect'.
    cRectangle m_rect;
    GuiTheme m_theme;
    void drawRectBorder(Color borderRect, Color borderBottomRight) const;
    void drawRectFillBorder(const GuiTheme& theme) const;
};
