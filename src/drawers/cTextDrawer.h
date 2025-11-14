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

struct textKey {
    std::string msg;
    Color color;
};

struct textKeyHash {
    std::size_t operator()(const textKey &k) const {
        return std::hash<std::string>()(k.msg) ^ std::hash<uint32_t>()( (k.color.r << 24) | (k.color.g <<16) | (k.color.b <<8) | (k.color.a) );
    }
};

struct textKeyEqual {
    bool operator()(const textKey &lhs, const textKey &rhs) const {
        return lhs.msg == rhs.msg && lhs.color.r == rhs.color.r && lhs.color.g == rhs.color.g &&
               lhs.color.b == rhs.color.b && lhs.color.a == rhs.color.a;
    }
};

struct textCacheEntry {
    SDL_Texture *texture;
    SDL_Texture *shadowsTexture;
    int width;
    int height;
    int lifeCounter;
};


class cTextDrawer {
public:
    cTextDrawer(TTF_Font *theFont);
    ~cTextDrawer();

    std::unique_ptr<textCacheEntry> createCacheEntry(Color color, const std::string &msg) const;

    void drawText(int x, int y, const std::string &msg, bool applyShadow = true) const;
    void drawText(cPoint &coords, Color color, const std::string &msg, bool applyShadow = true) const;
    void drawText(int x, int y, Color color, const std::string &msg, bool applyShadow = true) const;

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

    int getTextLength(const std::string &msg) const;
    int getFontHeight() const;
    void resetCache() const;

    cRectangle getRect(int x, int y, const std::string &msg) const;
    cRectangle *getAsRectangle(int x, int y, const std::string &msg) const;
protected:

private:
    TTF_Font *m_font;
    mutable std::unordered_map<textKey, std::unique_ptr<textCacheEntry>, textKeyHash, textKeyEqual> m_textCache;
};
