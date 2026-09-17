/**
 * @file GuiObject.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include "observers/cInputObserver.h"
#include "utils/cRectangle.h"
#include "gui/GuiTheme.hpp"

enum GuiRenderKind {
    OPAQUE_WITH_BORDER = 0,
    OPAQUE_WITHOUT_BORDER = 1,
    TRANSPARENT_WITH_BORDER = 2,
    TRANSPARENT_WITHOUT_BORDER = 3,
    WITH_TEXTURE =4,
    WITH_STRETCHED_TEXTURE = 5
};

enum GuiTextAlignHorizontal {
    LEFT,
    CENTER
};

class SDLDrawer;

class GuiObject : public cInputObserver {
public:
    virtual ~GuiObject() = default;
    virtual void draw() const = 0;
    virtual bool hasKeyboardFocus() const {
        return false;
    }

    void setTheme(const GuiTheme& theme);
    void setPosition(int x, int y);
    void setSize(int width, int height);
protected:
    explicit GuiObject(SDLDrawer* drawer, const cRectangle &rect);
    // any gui object has a position and size. Hence its always a 'rect'.
    cRectangle m_rect;
    GuiTheme m_theme;
    SDLDrawer* m_sdlDrawer = nullptr;
    void drawRectBorder(Color borderRect, Color borderBottomRight) const;
    void drawRectFillBorder(const GuiTheme& theme) const;
};
