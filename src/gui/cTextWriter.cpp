#include "cTextWriter.h"

// #include <alfont.h>

#include <cassert>

cTextWriter::cTextWriter(int x, int y, TTF_Font *theFont, int theFontSize)
{
    assert(theFont);
    assert(theFontSize > 0);
    originalX=x;
    drawX=x;
    originalY=y;
    drawY=y;
    font = theFont;
    fontSize = theFontSize;
    textDrawer = new cTextDrawer();
}

cTextWriter::~cTextWriter()
{
    font=nullptr;
    delete textDrawer;
}

void cTextWriter::write(const std::string &msg)
{
    textDrawer->drawText(drawX, drawY, msg);
    updateDrawY();
}

// void cTextWriter::write(const char *msg, int color)
// {
//     textDrawer->drawText(drawX, drawY, color, msg);
//     updateDrawY();
// }

void cTextWriter::write(const std::string &msg, Color color)
{
    textDrawer->drawText(drawX, drawY, color, msg);
    updateDrawY();
}

// void cTextWriter::writeWithOneInteger(const char *msg, int value1)
// {
//     textDrawer->drawTextWithOneInteger(drawX, drawY, msg, value1);
//     updateDrawY();
// }

// void cTextWriter::writeWithTwoIntegers(const char *msg, int value1, int value2)
// {
//     textDrawer->drawTextWithTwoIntegers(drawX, drawY, msg, value1, value2);
//     updateDrawY();
// }

void cTextWriter::updateDrawY()
{
    drawY += fontSize;
}
