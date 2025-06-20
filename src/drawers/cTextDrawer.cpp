#include "cTextDrawer.h"

#include "d2tmc.h"

// #include <alfont.h>
#include <allegro/color.h>

#include <cassert>

cTextDrawer::cTextDrawer(ALFONT_FONT *theFont)
{
    //Mira TEXT assert(theFont);
    font = theFont;
    applyShadow=true;
    textColor = makecol(255, 255, 255);
}

cTextDrawer::cTextDrawer() : cTextDrawer(small_font)
{
}

cTextDrawer::~cTextDrawer()
{
    font = NULL; // do not delete, because we are not the owner of it
}

void cTextDrawer::drawTextWithTwoIntegers(int x, int y, const char *msg, int var1, int var2) const
{
    //Mira TEXT if (applyShadow) {
    //Mira TEXT 	alfont_textprintf(bmp_screen, font, x + 1,y + 1, makecol(0,0,0), msg, var1, var2);
    //Mira TEXT }
    //Mira TEXT alfont_textprintf(bmp_screen, font, x,y, textColor, msg, var1, var2);
}

void cTextDrawer::drawTextWithOneInteger(int x, int y, const char *msg, int var) const
{
    drawTextWithOneInteger(x, y, textColor, msg, var);
}

void cTextDrawer::drawTextWithOneInteger(int x, int y, int color, const char *msg, int var) const
{
    //Mira TEXT if (applyShadow) {
    //Mira TEXT 	alfont_textprintf(bmp_screen, font, x + 1,y + 1, makecol(0,0,0), msg, var);
    //Mira TEXT }
    //Mira TEXT alfont_textprintf(bmp_screen, font, x,y, color, msg, var);
}

void cTextDrawer::drawText(int x, int y, int color, const char *msg) const
{
    //Mira TEXT if (applyShadow) {
    //Mira TEXT 	alfont_textprintf(bmp_screen, font, x + 1,y + 1, makecol(0,0,0), msg);
    //Mira TEXT }
    //Mira TEXT alfont_textprintf(bmp_screen, font, x,y, color, msg);
}

void cTextDrawer::drawText(cPoint &coords, int color, const char *msg) const
{
    drawText(coords.x, coords.y, color, msg);
}

void cTextDrawer::drawText(int x, int y, const char *msg) const
{
    drawText(x, y, textColor, msg);
}

void cTextDrawer::drawTextCentered(const char *msg, int y) const
{
    drawTextCentered(msg, y, textColor);
}

void cTextDrawer::drawTextCentered(const char *msg, int y, int color) const
{
    //Mira TEXT int lenghtInPixels = alfont_text_length(font, msg);
    //Mira TEXT int half = lenghtInPixels / 2;
    //Mira TEXT int xPos = (game.m_screenX / 2) - half;
    //Mira TEXT drawText(xPos, y, color, msg);
}

void cTextDrawer::drawTextCenteredInBox(const char *msg, int x, int y, int boxWidth, int boxHeight, int color) const
{
    //Mira TEXT int lenghtInPixels = alfont_text_length(font, msg);
    //Mira TEXT int heightInPixels = alfont_text_height(font);

    //Mira TEXT int halfLengthInPixels = lenghtInPixels / 2;
    //Mira TEXT int xPos = x + ((boxWidth / 2) - halfLengthInPixels);

    //Mira TEXT int halfHeightInPixels = heightInPixels / 2;
    //Mira TEXT int yPos = y + ((boxHeight / 2) - halfHeightInPixels);

    //Mira TEXT drawText(xPos, yPos, color, msg);
}

void cTextDrawer::drawTextCenteredInBox(const char *msg, const cRectangle &rect, int color) const
{
    drawTextCenteredInBox(msg, rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight(), color);
}

void cTextDrawer::drawTextCenteredInBox(const char *msg, const cRectangle &rect, int color, int offsetX, int offsetY) const
{
    drawTextCenteredInBox(msg, rect.getX() + offsetX, rect.getY() + offsetY, rect.getWidth(), rect.getHeight(), color);
}

void cTextDrawer::drawTextCentered(const char *msg, int x, int width, int y, int color) const
{
    //Mira TEXT int lenghtInPixels = alfont_text_length(font, msg);
    //Mira TEXT int half = lenghtInPixels / 2;
    //Mira TEXT int xPos = x + ((width / 2) - half);
    //Mira TEXT drawText(xPos, y, color, msg);
}

void cTextDrawer::drawTextBottomRight(const char *msg) const
{
    drawTextBottomRight(textColor, msg);
}

void cTextDrawer::drawTextBottomLeft(const char *msg) const
{
    drawTextBottomLeft(textColor, msg);
}

void cTextDrawer::drawTextBottomRight(int color, const char *msg) const
{
    //Mira TEXT int lenghtInPixels = alfont_text_length(font, msg);
    //Mira TEXT int x = game.m_screenX - lenghtInPixels;
    //Mira TEXT int y = game.m_screenY - getFontHeight();
    //Mira TEXT drawText(x, y, color, msg);
}

int cTextDrawer::getFontHeight() const
{
    //Mira TEXT assert(font && "Font not set!?");
    //Mira TEXT return alfont_text_height(font);
    return 10;
}

void cTextDrawer::drawTextBottomLeft(int color, const char *msg) const
{
    //Mira TEXT int y = game.m_screenY - alfont_text_height(font);
    //Mira TEXT drawText(0, y, color, msg);
}

int cTextDrawer::textLength(const char *msg) const
{
    //Mira TEXT return alfont_text_length(font, msg);
    return 0;
}

void cTextDrawer::setFont(ALFONT_FONT *theFont)
{
    if (theFont == nullptr) return; // do not allow null
    this->font = theFont;
}

cRectangle cTextDrawer::getRect(int x, int y, const char *msg) const
{
    return cRectangle(x, y, textLength(msg), getFontHeight());
}

cRectangle *cTextDrawer::getAsRectangle(int x, int y, const char *msg) const
{
    return new cRectangle(x, y, textLength(msg), getFontHeight());
}

void cTextDrawer::drawText(int x, int y, const char *msg, const char *var) const
{
    drawText(x, y, textColor, msg, var);
}

void cTextDrawer::drawText(int x, int y, int color, const char *msg, const char *var) const
{
    //Mira TEXT if (applyShadow) {
    //Mira TEXT     alfont_textprintf(bmp_screen, font, x + 1,y + 1, makecol(0,0,0), msg, var);
    //Mira TEXT }
    //Mira TEXT alfont_textprintf(bmp_screen, font, x,y, color, msg, var);
}

void cTextDrawer::drawText(int x, int y, int color, const char *msg, int var) const
{
    drawTextWithOneInteger(x, y, color, msg, var);
}

void cTextDrawer::drawText(int x, int y, const char *msg, int var1, int var2) const
{
    drawTextWithTwoIntegers(x, y, msg, var1, var2);
}

void cTextDrawer::drawText(int x, int y, const char *msg, int var) const
{
    drawText(x, y, textColor, msg, var);
}
