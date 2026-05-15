#include "cTextWriter.h"
#include "drawers/cTextDrawer.h"
#include "include/cAssert.h"

cTextWriter::cTextWriter(cTextDrawer* _textDrawer, int theFontSize)
{
    my_assert(theFontSize > 0);
    my_assert(_textDrawer != nullptr);
    drawX=-1;
    drawY=-1;
    fontSize = theFontSize;
    m_textDrawer = _textDrawer;
}

cTextWriter::~cTextWriter()
{}

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
