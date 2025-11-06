#include "cTextWriter.h"
#include <cassert>

cTextWriter::cTextWriter(cTextDrawer* _textDrawer,/*TTF_Font *theFont,*/ int theFontSize)
{
    // assert(theFont);
    assert(theFontSize > 0);
    // originalX=x;
    drawX=-1;
    // originalY=y;
    drawY=-1;
    // font = theFont;
    fontSize = theFontSize;
    textDrawer = _textDrawer;//new cTextDrawer();
}

cTextWriter::~cTextWriter()
{
    // font=nullptr;
    delete textDrawer;
}

void cTextWriter::moveTo(int x, int y)
{
    drawX = x; 
    drawY = y;     
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
