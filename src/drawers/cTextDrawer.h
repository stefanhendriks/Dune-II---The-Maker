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
#include <unordered_map>
#include <memory>

class cTextTextureCache;

class cTextDrawer {
public:
    // Constructor, initializes the drawer with a TTF font
    cTextDrawer(TTF_Font *theFont);
    ~cTextDrawer();

    // Draws text at (x, y) in white, with optional shadow
    void drawText(int x, int y, const std::string &msg, bool applyShadow = true) const;
    // Draws text at given coordinates, with color and optional shadow
    void drawText(cPoint &coords, Color color, const std::string &msg, bool applyShadow = true) const;
    // Draws text at (x, y) with color and optional shadow
    void drawText(int x, int y, Color color, const std::string &msg, bool applyShadow = true) const;
    // Draws centered text horizontally at line y, in white
    void drawTextCentered(const std::string &msg, int y) const;
    // Draws centered text horizontally at line y, with color
    void drawTextCentered(const std::string &msg, int y, Color color) const;
    // Draws centered text within a region (x, width) at line y, with color
    void drawTextCentered(const std::string &msg, int x, int width, int y, Color color) const;

    /**
     * Draws the text within a 'box' (rectangle), positioned at X,Y. The width/height of the box determine
     * how to center the text. When centering text, the height/width of the font is taken into account.
    */
    // Draws centered text inside a box (x, y, boxWidth, boxHeight), with color
    void drawTextCenteredInBox(const std::string &msg, int x, int y, int boxWidth, int boxHeight, Color color) const;
    // Draws centered text inside a rectangle, with color
    void drawTextCenteredInBox(const std::string &msg, const cRectangle &rect, Color color) const;
    // Draws centered text inside a rectangle, with color and offset
    void drawTextCenteredInBox(const std::string &msg, const cRectangle &rect, Color color, int offsetX, int offsetY) const;
    // Draws text at the bottom right of the screen, in white, with margin
    void drawTextBottomRight(const std::string &msg, int margin=0) const;
    // Draws text at the bottom left of the screen, in white, with margin
    void drawTextBottomLeft(const std::string &msg, int margin=0) const;
    // Draws text at the bottom right of the screen, with color and margin
    void drawTextBottomRight(Color color, const std::string &msg,int margin =0) const;
    // Draws text at the bottom left of the screen, with color and margin
    void drawTextBottomLeft(Color color, const std::string &msg,int margin =0) const;
    // Returns the pixel length of the text
    int getTextLength(const std::string &msg) const;
    // Returns the font height in pixels
    int getFontHeight() const;
    // Clears the text texture cache
    void resetCache() const;
    // Returns a rectangle bounding the text at (x, y)
    cRectangle getRect(int x, int y, const std::string &msg) const;
    // Returns a pointer to a rectangle bounding the text at (x, y)
    cRectangle *getAsRectangle(int x, int y, const std::string &msg) const;
protected:

private:
    TTF_Font *m_font;
    std::unique_ptr<cTextTextureCache> m_textCache;
};
