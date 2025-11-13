#include "cTextDrawer.h"

#include "d2tmc.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Color.hpp"
#include <iostream>
#include <cassert>

cTextDrawer::cTextDrawer(TTF_Font *theFont) :
    m_font(theFont)
{
}

cTextDrawer::~cTextDrawer()
{
    for (auto &pair : m_textCache) {
        if (pair.second->texture) {
            SDL_DestroyTexture(pair.second->texture);
        }
        if (pair.second->shadowsTexture) {
            SDL_DestroyTexture(pair.second->shadowsTexture);
        }
    }
    m_font = nullptr; // do not delete, because we are not the owner of it
}

void cTextDrawer::drawText(int x, int y, Color color, const std::string &msg, bool applyShadow) const
{
    if (msg.empty()) return;

    auto textKeyInstance = textKey{msg, color};
    auto it = m_textCache.find(textKeyInstance);
    if (it == m_textCache.end()) {
        // not found, create it
        auto newCacheEntry = std::make_unique<textCacheEntry>();
        // create shadow texture if needed
        SDL_Surface *textSurface = TTF_RenderText_Blended(m_font, msg.c_str(), Color::black().toSDL());
        newCacheEntry->shadowsTexture = SDL_CreateTextureFromSurface(renderDrawer->getRenderer(), textSurface);
        SDL_FreeSurface(textSurface);
        // create main texture
        textSurface = TTF_RenderText_Blended(m_font, msg.c_str(), color.toSDL());
        newCacheEntry->texture = SDL_CreateTextureFromSurface(renderDrawer->getRenderer(), textSurface);
        newCacheEntry->width = textSurface->w;
        newCacheEntry->height = textSurface->h;
        newCacheEntry->lifeCounter = 10;
        SDL_FreeSurface(textSurface);
        // store in cache at end
        auto result = m_textCache.emplace(textKeyInstance, std::move(newCacheEntry));
        it = result.first;
    }
    // draw it yet.
    auto &cacheEntry = it->second;
    it->second->lifeCounter += 1;
    if (applyShadow) {
        renderDrawer->renderTexture(cacheEntry->shadowsTexture, x + 1, y + 1,cacheEntry->width, cacheEntry->height);
    }
    renderDrawer->renderTexture(cacheEntry->texture, x, y,cacheEntry->width, cacheEntry->height);
}

void cTextDrawer::drawText(cPoint &coords, Color color, const std::string &msg, bool applyShadow) const
{
    drawText(coords.x, coords.y, color, msg, applyShadow);
}

void cTextDrawer::drawText(int x, int y, const std::string &msg, bool applyShadow) const
{
    drawText(x, y, Color::white() , msg, applyShadow);
}

void cTextDrawer::drawTextCentered(const std::string &msg, int y) const
{
    drawTextCentered(msg, y, Color::white());
}

void cTextDrawer::drawTextCentered(const std::string &msg, int y, Color color) const
{
    if (msg.empty()) return;
    int w,h;
    TTF_SizeUTF8(m_font, msg.c_str(), &w, &h);
    int half = w / 2;
    int xPos = (game.m_screenW / 2) - half;
    drawText(xPos, y, color, msg);
}

void cTextDrawer::drawTextCenteredInBox(const std::string &msg, int x, int y, int boxWidth, int boxHeight, Color color) const
{
    if (msg.empty()) return;
    int w,h;
    TTF_SizeUTF8(m_font, msg.c_str(), &w, &h);
    int lenghtInPixels = w;
    int heightInPixels = h;

    int halfLengthInPixels = lenghtInPixels / 2;
    int xPos = x + ((boxWidth / 2) - halfLengthInPixels);
    int halfHeightInPixels = heightInPixels / 2;
    int yPos = y + ((boxHeight / 2) - halfHeightInPixels);
    drawText(xPos, yPos, color, msg);
}

void cTextDrawer::drawTextCenteredInBox(const std::string &msg, const cRectangle &rect, Color color) const
{
    drawTextCenteredInBox(msg, rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight(), color);
}

void cTextDrawer::drawTextCenteredInBox(const std::string &msg, const cRectangle &rect, Color color, int offsetX, int offsetY) const
{
    drawTextCenteredInBox(msg, rect.getX() + offsetX, rect.getY() + offsetY, rect.getWidth(), rect.getHeight(), color);
}

void cTextDrawer::drawTextCentered(const std::string &msg, int x, int width, int y, Color color) const
{
    if (msg.empty()) return;
    int w,h;
    TTF_SizeUTF8(m_font, msg.c_str(), &w, &h);
    int lenghtInPixels = w;
    int half = lenghtInPixels / 2;
    int xPos = x + ((width / 2) - half);
    drawText(xPos, y, color, msg);
}

void cTextDrawer::drawTextBottomRight(const std::string &msg, int margin) const
{
    drawTextBottomRight(Color::white(), msg, margin);
}

void cTextDrawer::drawTextBottomLeft(const std::string &msg, int margin) const
{
    drawTextBottomLeft(Color::white(), msg, margin);
}

void cTextDrawer::drawTextBottomRight(Color color, const std::string &msg, int margin) const
{
    if (msg.empty()) return;
    int w,h;
    TTF_SizeUTF8(m_font, msg.c_str(), &w, &h);
    int lenghtInPixels = w;
    int x = game.m_screenW - lenghtInPixels-margin;
    int y = game.m_screenH - getFontHeight()-20-margin;
    drawText(x, y, color, msg);
}

int cTextDrawer::getFontHeight() const
{
    return TTF_FontHeight(m_font);
}

void cTextDrawer::drawTextBottomLeft(Color color, const std::string &msg, int margin) const
{
    if (msg.empty()) return;
    int w,h;
    TTF_SizeUTF8(m_font, msg.c_str(), &w, &h);
    int y = game.m_screenH - h-20-margin;
    drawText(margin, y, color, msg);
}

int cTextDrawer::getTextLength(const std::string &msg) const
{
    int w,h;
    TTF_SizeUTF8(m_font, msg.c_str(), &w, &h);
    return w;
}

cRectangle cTextDrawer::getRect(int x, int y, const std::string &msg) const
{
    return cRectangle(x, y, getTextLength(msg), getFontHeight());
}

cRectangle *cTextDrawer::getAsRectangle(int x, int y, const std::string &msg) const
{
    return new cRectangle(x, y, getTextLength(msg), getFontHeight());
}

void cTextDrawer::resetCache() const {
    auto it = m_textCache.begin(); 
    while (it != m_textCache.end()) {
        auto cacheEntry = std::move(it->second);

        if (cacheEntry->lifeCounter == 0) {
            if (cacheEntry->texture) {
                SDL_DestroyTexture(cacheEntry->texture);
            }
            if (cacheEntry->shadowsTexture) {
                SDL_DestroyTexture(cacheEntry->shadowsTexture);
            }
            it = m_textCache.erase(it); 
         } else {
            cacheEntry->lifeCounter = 0;
            ++it; 
        }
    }
}
