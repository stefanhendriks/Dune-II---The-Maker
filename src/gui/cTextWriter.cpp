/**
 * @file cTextWriter.cpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#include "cTextWriter.h"
#include "drawers/cTextDrawer.h"
#include "include/cAssert.h"

cTextWriter::cTextWriter(cTextDrawer* _textDrawer, int theFontSize)
{
    d2tm_assert(theFontSize > 0);
    d2tm_assert(_textDrawer != nullptr);
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
