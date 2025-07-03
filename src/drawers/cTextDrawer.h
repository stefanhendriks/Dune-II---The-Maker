/*
 * TextDrawer.h
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

#pragma once

#include "utils/cRectangle.h"
#include "utils/Color.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class cTextDrawer {
public:
    cTextDrawer();
    cTextDrawer(TTF_Font *theFont);
    ~cTextDrawer();

    void setFont(TTF_Font *theFont);

    void drawText(int x, int y, const std::string &msg) const;
    void drawText(cPoint &coords, Color color, const std::string &msg) const;
    void drawText(int x, int y, Color color, const std::string &msg) const;

    void drawTextCentered(const std::string &msg, int y) const;
    void drawTextCentered(const std::string &msg, int y, Color color) const;
    void drawTextCentered(const std::string &msg, int x, int width, int y, Color color) const;

    /**
     * Draws the text within a 'box' (rectangle), positioned at X,Y. The width/height of the box determine
     * how to center the text. When centering text, the height/width of the font is taken into account.
     *
     * @param msg
     * @param x
     * @param y
     * @param boxWidth
     * @param boxHeight
     * @param color
     */
    void drawTextCenteredInBox(const std::string &msg, int x, int y, int boxWidth, int boxHeight, Color color) const;
    void drawTextCenteredInBox(const std::string &msg, const cRectangle &rect, Color color) const;
    void drawTextCenteredInBox(const std::string &msg, const cRectangle &rect, Color color, int offsetX, int offsetY) const;

    void drawTextBottomRight(const std::string &msg, int margin=0) const;
    void drawTextBottomLeft(const std::string &msg, int margin=0) const;

    void drawTextBottomRight(Color color, const std::string &msg,int margin =0) const;
    void drawTextBottomLeft(Color color, const std::string &msg,int margin =0) const;

    void setApplyShadow(bool value) {
        applyShadow = value;
    }

    void setTextColor(Color value) {
        textColor = value;
    }
    int textLength(const std::string &msg) const;
    int getFontHeight() const;

    cRectangle getRect(int x, int y, const std::string &msg) const;
    cRectangle *getAsRectangle(int x, int y, const std::string &msg) const;
protected:

private:
    TTF_Font *font;
    bool applyShadow;
    Color textColor;
};
