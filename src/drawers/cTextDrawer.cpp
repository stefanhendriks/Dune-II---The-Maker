#include "cTextDrawer.h"
#include "drawers/cTextTextureCache.h"
#include "game/cGameSettings.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Color.hpp"
#include <iostream>
#include "include/cAssert.h"

cTextDrawer::cTextDrawer(TTF_Font *theFont, cGameSettings *settings, SDLDrawer *renderer) :
    m_font(theFont),
    m_textCache(std::make_unique<cTextTextureCache>(theFont, renderer)),
    m_settings(settings),
    m_renderer(renderer)
{
    d2tm_assert(theFont != nullptr);
    d2tm_assert(settings != nullptr);
    d2tm_assert(renderer != nullptr);
}

cTextDrawer::~cTextDrawer()
{
    m_font = nullptr;
}

void cTextDrawer::drawText(int x, int y, Color color, const std::string &msg, bool applyShadow) const
{
    if (msg.empty()) return;
    auto cacheEntry = m_textCache->findOrCreate(color, msg);
    if (!cacheEntry) return;
    cacheEntry->lifeCounter += 1;
    if (applyShadow) {
        m_renderer->renderTexture(cacheEntry->shadowsTexture, x + 1, y + 1,cacheEntry->width, cacheEntry->height);
    }
    m_renderer->renderTexture(cacheEntry->texture, x, y,cacheEntry->width, cacheEntry->height);
}

void cTextDrawer::drawText(cPoint &coords, Color color, const std::string &msg, bool applyShadow) const
{
    drawText(coords.x, coords.y, color, msg, applyShadow);
}

void cTextDrawer::drawText(int x, int y, const std::string &msg, bool applyShadow) const
{
    drawText(x, y, Color::White , msg, applyShadow);
}

void cTextDrawer::drawTextCentered(const std::string &msg, int y) const
{
    drawTextCentered(msg, y, Color::White);
}

void cTextDrawer::drawTextCentered(const std::string &msg, int y, Color color) const
{
    if (msg.empty()) return;
    int w,h;
    TTF_GetStringSize(m_font, msg.c_str(), 0, &w, &h);
    int half = w / 2;
    int xPos = (m_settings->getScreenW() / 2) - half;
    drawText(xPos, y, color, msg);
}

void cTextDrawer::drawTextCenteredInBox(const std::string &msg, int x, int y, int boxWidth, int boxHeight, Color color) const
{
    if (msg.empty()) return;
    int w,h;
    TTF_GetStringSize(m_font, msg.c_str(), 0, &w, &h);
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
    TTF_GetStringSize(m_font, msg.c_str(), 0, &w, &h);
    int lenghtInPixels = w;
    int half = lenghtInPixels / 2;
    int xPos = x + ((width / 2) - half);
    drawText(xPos, y, color, msg);
}

void cTextDrawer::drawTextBottomRight(const std::string &msg, int margin) const
{
    drawTextBottomRight(Color::White, msg, margin);
}

void cTextDrawer::drawTextBottomLeft(const std::string &msg, int margin) const
{
    drawTextBottomLeft(Color::White, msg, margin);
}

void cTextDrawer::drawTextBottomRight(Color color, const std::string &msg, int margin) const
{
    if (msg.empty()) return;
    int w,h;
    TTF_GetStringSize(m_font, msg.c_str(), 0, &w, &h);
    int lenghtInPixels = w;
    int x = m_settings->getScreenW() - lenghtInPixels-margin;
    int y = m_settings->getScreenH() - getFontHeight()-20-margin;
    drawText(x, y, color, msg);
}

int cTextDrawer::getFontHeight() const
{
    return TTF_GetFontHeight(m_font);
}

void cTextDrawer::drawTextBottomLeft(Color color, const std::string &msg, int margin) const
{
    if (msg.empty()) return;
    int w,h;
    TTF_GetStringSize(m_font, msg.c_str(), 0, &w, &h);
    int y = m_settings->getScreenH() - h-20-margin;
    drawText(margin, y, color, msg);
}

int cTextDrawer::getTextLength(const std::string &msg) const
{
    int w,h;
    TTF_GetStringSize(m_font, msg.c_str(), 0, &w, &h);
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

void cTextDrawer::resetCache() const
{
    m_textCache->resetCache();
}
