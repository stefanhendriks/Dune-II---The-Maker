/*
 * TextDrawer.h
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

#pragma once

#include "utils/cRectangle.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class cTextDrawer {
public:
    cTextDrawer();
    cTextDrawer(TTF_Font *theFont);
    ~cTextDrawer();

    void setFont(TTF_Font *theFont);

    // void drawTextWithOneInteger(int x, int y, SDL_Color color, const std::string& msg, int var) const;
    // void drawTextWithOneInteger(int x, int y, const std::string& msg, int var) const;
    // void drawTextWithTwoIntegers(int x, int y, const std::string& msg, int var1, int var2) const;

    // void drawText(int x, int y, const std::string& msg, const char *var) const;
    // void drawText(int x, int y, const std::string& msg, int var) const;
    void drawText(int x, int y, const std::string &msg) const;

    void drawText(cPoint &coords, SDL_Color color, const std::string &msg) const;

    //void drawText(int x, int y, const std::string& msg, int var1, int var2) const;
    void drawText(int x, int y, SDL_Color color, const std::string &msg) const;
    //void drawText(int x, int y, SDL_Color color, const std::string& msg, int var) const;
    //void drawText(int x, int y, SDL_Color color, const std::string& msg, const char *var) const;

    void drawTextCentered(const std::string &msg, int y) const;
    void drawTextCentered(const std::string &msg, int y, SDL_Color color) const;
    void drawTextCentered(const std::string &msg, int x, int width, int y, SDL_Color color) const;

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
    void drawTextCenteredInBox(const std::string &msg, int x, int y, int boxWidth, int boxHeight, SDL_Color color) const;
    void drawTextCenteredInBox(const std::string &msg, const cRectangle &rect, SDL_Color color) const;
    void drawTextCenteredInBox(const std::string &msg, const cRectangle &rect, SDL_Color color, int offsetX, int offsetY) const;

    void drawTextBottomRight(const std::string &msg) const;
    void drawTextBottomLeft(const std::string &msg) const;

    void drawTextBottomRight(SDL_Color color, const std::string &msg) const;
    void drawTextBottomLeft(SDL_Color color, const std::string &msg) const;

    void setApplyShadow(bool value) {
        applyShadow = value;
    }

    void setTextColor(SDL_Color value) {
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
    SDL_Color textColor;
};
