#include "cTextWriter.h"
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

void cTextWriter::write(const std::string &msg, Color color)
{
    textDrawer->drawText(drawX, drawY, color, msg);
    updateDrawY();
}

void cTextWriter::updateDrawY()
{
    drawY += fontSize;
}
