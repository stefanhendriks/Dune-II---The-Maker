#include "gui/GuiObject.h"
#include "d2tmc.h"
#include "drawers/SDLDrawer.hpp"
#include <iostream>

void GuiObject::setTheme(const GuiTheme& theme)
{
    m_theme = theme;
}

void GuiObject::drawRectBorder(Color borderRect, Color borderBottomRight) const
{
    int x1= m_rect.getX();
    int y1 = m_rect.getY();
    int width = m_rect.getWidth();
    int height = m_rect.getHeight();
    //std::cout <<":" << x1 << " " << y1 << " " << width << " " << height << std::endl;
    renderDrawer->renderRectColor(m_rect,borderRect);
    renderDrawer->renderLine(x1+width, y1, x1+width, y1+height,borderBottomRight);
    renderDrawer->renderLine(x1, y1+height, x1+width, y1+height, borderBottomRight);

    // renderChangeColor(gui_colorBorderLight);
    // SDL_RenderDrawRect(renderer, &tmp);
    // renderChangeColor(gui_colorBorderDark);
    // SDL_RenderDrawLine(renderer, x1+width, y1, x1+width, y1+height);
    // renderChangeColor(gui_colorBorderDark);
    // SDL_RenderDrawLine(renderer, x1, y1+height, x1+width, y1+height);
}

void GuiObject::drawRectFillBorder(const GuiTheme& theme) const
{
    renderDrawer->renderRectFillColor(m_rect, theme.fillColor);
    drawRectBorder(theme.borderLight, theme.borderDark);
}