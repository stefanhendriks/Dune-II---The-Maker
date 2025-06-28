#include "cTextDrawer.h"

#include "d2tmc.h"
#include "drawers/SDLDrawer.hpp"
//#include <iostream>

#include <cassert>

cTextDrawer::cTextDrawer(TTF_Font *theFont)
{
    assert(theFont);
    font = theFont;
    applyShadow=true;
    textColor = SDL_Color{255, 255, 255,255};
}

cTextDrawer::cTextDrawer() : cTextDrawer(small_font)
{}

cTextDrawer::~cTextDrawer()
{
    font = nullptr; // do not delete, because we are not the owner of it
}

// void cTextDrawer::drawTextWithTwoIntegers(int x, int y, const std::string& msg, int var1, int var2) const
// {
//     //Mira TEXT if (applyShadow) {
//     //Mira TEXT 	alfont_textprintf(bmp_screen, font, x + 1,y + 1, SDL_Color{0,0,0), msg, var1, var2);
//     //Mira TEXT }
//     //Mira TEXT alfont_textprintf(bmp_screen, font, x,y, textColor, msg, var1, var2);
// }

// void cTextDrawer::drawTextWithOneInteger(int x, int y, const std::string& msg, int var) const
// {
//     drawTextWithOneInteger(x, y, textColor, msg, var);
// }

// void cTextDrawer::drawTextWithOneInteger(int x, int y, SDL_Color color, const std::string& msg, int var) const
// {
//     //Mira TEXT if (applyShadow) {
//     //Mira TEXT 	alfont_textprintf(bmp_screen, font, x + 1,y + 1, SDL_Color{0,0,0), msg, var);
//     //Mira TEXT }
//     //Mira TEXT alfont_textprintf(bmp_screen, font, x,y, color, msg, var);
// }

void cTextDrawer::drawText(int x, int y, SDL_Color color, const std::string &msg) const
{
    if (msg.empty()) return;
    if (applyShadow) {
        SDL_Surface *texte = TTF_RenderText_Blended(font, msg.c_str(), SDL_Color{0,0,0,255});
        assert(texte);
        renderDrawer->renderFromSurface(texte,x+1,y+1);
        SDL_FreeSurface(texte);
    }
    SDL_Surface *texte = TTF_RenderText_Blended(font, msg.c_str(), color);
    renderDrawer->renderFromSurface(texte,x,y);
    SDL_FreeSurface(texte);
}

void cTextDrawer::drawText(cPoint &coords, SDL_Color color, const std::string &msg) const
{
    drawText(coords.x, coords.y, color, msg);
}

void cTextDrawer::drawText(int x, int y, const std::string &msg) const
{
    drawText(x, y, textColor, msg);
}

void cTextDrawer::drawTextCentered(const std::string &msg, int y) const
{
    drawTextCentered(msg, y, textColor);
}

void cTextDrawer::drawTextCentered(const std::string &msg, int y, SDL_Color color) const
{
    if (msg.empty()) return;
    int w,h;
    TTF_SizeUTF8(font, msg.c_str(), &w, &h);
    int half = w / 2;
    int xPos = (game.m_screenW / 2) - half;
    //std::cout << xPos << y << msg <<std::endl;
    drawText(xPos, y, color, msg);
}

void cTextDrawer::drawTextCenteredInBox(const std::string &msg, int x, int y, int boxWidth, int boxHeight, SDL_Color color) const
{
    if (msg.empty()) return;
    int w,h;
    TTF_SizeUTF8(font, msg.c_str(), &w, &h);
    int lenghtInPixels = w;
    int heightInPixels = h;

    int halfLengthInPixels = lenghtInPixels / 2;
    int xPos = x + ((boxWidth / 2) - halfLengthInPixels);
    int halfHeightInPixels = heightInPixels / 2;
    int yPos = y + ((boxHeight / 2) - halfHeightInPixels);
    drawText(xPos, yPos, color, msg);
}

void cTextDrawer::drawTextCenteredInBox(const std::string &msg, const cRectangle &rect, SDL_Color color) const
{
    drawTextCenteredInBox(msg, rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight(), color);
}

void cTextDrawer::drawTextCenteredInBox(const std::string &msg, const cRectangle &rect, SDL_Color color, int offsetX, int offsetY) const
{
    drawTextCenteredInBox(msg, rect.getX() + offsetX, rect.getY() + offsetY, rect.getWidth(), rect.getHeight(), color);
}

void cTextDrawer::drawTextCentered(const std::string &msg, int x, int width, int y, SDL_Color color) const
{
    if (msg.empty()) return;
    //std::cout << msg << std::endl;
    int w,h;
    TTF_SizeUTF8(font, msg.c_str(), &w, &h);
    int lenghtInPixels = w;
    int half = lenghtInPixels / 2;
    int xPos = x + ((width / 2) - half);
    drawText(xPos, y, color, msg);
}

void cTextDrawer::drawTextBottomRight(const std::string &msg) const
{
    drawTextBottomRight(textColor, msg);
}

void cTextDrawer::drawTextBottomLeft(const std::string &msg) const
{
    drawTextBottomLeft(textColor, msg);
}

void cTextDrawer::drawTextBottomRight(SDL_Color color, const std::string &msg) const
{
    if (msg.empty()) return;
    int w,h;
    TTF_SizeUTF8(font, msg.c_str(), &w, &h);
    int lenghtInPixels = w;
    int x = game.m_screenW - lenghtInPixels;
    int y = game.m_screenH - getFontHeight()-20;
    drawText(x, y, color, msg);
}

int cTextDrawer::getFontHeight() const
{
    //Mira TEXT assert(font && "Font not set!?");
    //Mira TEXT return alfont_text_height(font);
    return TTF_FontHeight(font);
}

void cTextDrawer::drawTextBottomLeft(SDL_Color color, const std::string &msg) const
{
    if (msg.empty()) return;
    int w,h;
    TTF_SizeUTF8(font, msg.c_str(), &w, &h);
    int y = game.m_screenH - h-20;
    drawText(0, y, color, msg);
}

int cTextDrawer::textLength(const std::string &msg) const
{
    //Mira TEXT return alfont_text_length(font, msg);
    int w,h;
    TTF_SizeUTF8(font, msg.c_str(), &w, &h);
    return w;
}

void cTextDrawer::setFont(TTF_Font *theFont)
{
    if (theFont == nullptr) return; // do not allow null
    this->font = theFont;
}

cRectangle cTextDrawer::getRect(int x, int y, const std::string &msg) const
{
    return cRectangle(x, y, textLength(msg), getFontHeight());
}

cRectangle *cTextDrawer::getAsRectangle(int x, int y, const std::string &msg) const
{
    return new cRectangle(x, y, textLength(msg), getFontHeight());
}

// void cTextDrawer::drawText(int x, int y, const std::string& msg, const char *var) const
// {
//     drawText(x, y, textColor, msg, var);
// }

// void cTextDrawer::drawText(int x, int y, SDL_Color color, const std::string& msg, const char *var) const
// {
//Mira TEXT if (applyShadow) {
//Mira TEXT     alfont_textprintf(bmp_screen, font, x + 1,y + 1, SDL_Color{0,0,0), msg, var);
//Mira TEXT }
//Mira TEXT alfont_textprintf(bmp_screen, font, x,y, color, msg, var);
// }

// void cTextDrawer::drawText(int x, int y, SDL_Color color, const std::string& msg, int var) const
// {
//     drawTextWithOneInteger(x, y, color, msg, var);
// }

// void cTextDrawer::drawText(int x, int y, const std::string& msg, int var1, int var2) const
// {
//     drawTextWithTwoIntegers(x, y, msg, var1, var2);
// }

// void cTextDrawer::drawText(int x, int y, const std::string& msg, int var) const
// {
//     drawText(x, y, textColor, msg, var);
// }
