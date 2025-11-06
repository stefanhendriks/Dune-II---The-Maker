#include "cTextWriter.h"
#include <cassert>

cTextWriter::cTextWriter(cTextDrawer* _textDrawer, int theFontSize)
{
    assert(theFontSize > 0);
    drawX=-1;
    drawY=-1;
    fontSize = theFontSize;
    m_textDrawer = _textDrawer;
}

cTextWriter::~cTextWriter()
{
    delete m_textDrawer;
}

void cTextWriter::moveTo(int x, int y)
{
    drawX = x; 
    drawY = y;     
}

void cTextWriter::write(const std::string &msg)
{
    m_textDrawer->drawText(drawX, drawY, msg);
    updateDrawY();
}

void cTextWriter::write(const std::string &msg, Color color)
{
    m_textDrawer->drawText(drawX, drawY, color, msg);
    updateDrawY();
}

void cTextWriter::updateDrawY()
{
    drawY += fontSize;
}
