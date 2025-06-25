#include "gui.h"

#include "d2tmc.h"
#include "drawers/SDLDrawer.hpp"

#include <SDL2/SDL.h>

bool GUI_DRAW_FRAME_PRESSED(int x1, int y1, int width, int height)
{
    // fill it up
    renderDrawer->renderRectFillColor(x1, y1, width, height, 176,176,196,255);

    // _rect
    //_rect(bmp_screen, x1,y1,x1+width, y1+height, SDL_Color{84,84,120));
    renderDrawer->renderRectColor(x1, y1, width, height, 84,84,120,255);

    // lines to darken the right sides
    // _line(bmp_screen, x1+width, y1, x1+width , y1+height, SDL_Color{252,252,252));
    renderDrawer->renderLine(x1+width, y1, x1+width, y1+height, SDL_Color{252,252,252,255});
    // _line(bmp_screen, x1, y1+height, x1+width , y1+height, SDL_Color{252,252,252));
    renderDrawer->renderLine(x1, y1+height, x1+width, y1+height, SDL_Color{252,252,252,255});

    // if ((mouse_x >= x1 && mouse_x < (x1+width)) && (mouse_y >= y1 && mouse_y <= (y1+height)))
    return MOUSE_WITHIN_RECT(x1, y1, width, height);
}

bool MOUSE_WITHIN_RECT(int x, int y, int width, int height)
{
    auto m_mouse = game.getMouse();
    return ((m_mouse->getX() >= x && m_mouse->getX() < (x + width))
            && (m_mouse->getY() >= y && m_mouse->getY() <= (y + height)));
}

/**
 * Draws a text at x, y; when  mouse hovers over it, it will render the text with hoverColor, else with white.
 *
 * It will return true when mouse hovers above it.
 *
 * @param x
 * @param y
 * @param text
 * @param hoverColor
 *
 * @return true if mouse is hovering over this
 */
bool GUI_DRAW_BENE_TEXT_MOUSE_SENSITIVE(int x, int y, const std::string &text, SDL_Color hoverColor)
{
    const char *cstring = text.c_str();

    //Mira TEXT alfont_textprintf(bmp_screen, bene_font, x + 1, y + 1, SDL_Color{0, 0, 0), cstring);

    //Mira TEXT int width = alfont_text_length(bene_font, cstring);
    //Mira TEXT int height = alfont_text_height(bene_font);
    //Mira TEXT if (MOUSE_WITHIN_RECT(x, y, width, height)) {
    //Mira TEXT alfont_textprintf(bmp_screen, bene_font, x, y, hoverColor, cstring);
    //Mira TEXT     return true;
    //Mira TEXT }
    //Mira TEXT     alfont_textprintf(bmp_screen, bene_font, x, y, SDL_Color{255, 255, 255), cstring);
    return false;
}

bool GUI_DRAW_FRAME(int x, int y, int width, int height)
{
    cRectangle rect = cRectangle(x, y, width, height);
    renderDrawer->gui_DrawRect(rect);

//    // fill it up
//    _rectfill(bmp_screen, x, y, x+width, y+height, SDL_Color{176,176,196));
//
//    // rect
//    _rect(bmp_screen, x,y,x+width, y+height, SDL_Color{252,252,252));
//
//    // lines to darken the right sides
//    _line(bmp_screen, x+width, y, x+width , y+height, SDL_Color{84,84,120));
//    _line(bmp_screen, x, y+height, x+width , y+height, SDL_Color{84,84,120));
    auto m_mouse = game.getMouse();
    if ((m_mouse->getX() >= x && m_mouse->getX() < (x + width))
            && (m_mouse->getY() >= y && m_mouse->getY() <= (y + height))) {
        return true;
    }


    return false; // not hovering on it
}
