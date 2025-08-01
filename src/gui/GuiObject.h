#pragma once

#include "observers/cInputObserver.h"
#include "utils/cRectangle.h"
#include "gui/GuiTheme.hpp"

enum GuiRenderKind {
    OPAQUE_WITH_BORDER = 0,
    OPAQUE_WITHOUT_BORDER = 1,
    TRANSPARENT_WITH_BORDER = 2,
    TRANSPARENT_WITHOUT_BORDER = 3,
    WITH_TEXTURE =4
};

enum GuiTextAlignHorizontal {
    LEFT,
    CENTER
};

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
