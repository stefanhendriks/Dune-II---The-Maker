#include "gui.h"

#include "d2tmc.h"
#include "drawers/cAllegroDrawer.h"

#include <alfont.h>
#include <allegro.h>

bool GUI_DRAW_FRAME_PRESSED(int x1, int y1, int width, int height) {
    // fill it up
    rectfill(bmp_screen, x1, y1, x1+width, y1+height, makecol(176,176,196));

    // rect
    rect(bmp_screen, x1,y1,x1+width, y1+height, makecol(84,84,120));

    // lines to darken the right sides
    line(bmp_screen, x1+width, y1, x1+width , y1+height, makecol(252,252,252));
    line(bmp_screen, x1, y1+height, x1+width , y1+height, makecol(252,252,252));

    // if ((mouse_x >= x1 && mouse_x < (x1+width)) && (mouse_y >= y1 && mouse_y <= (y1+height)))
    return MOUSE_WITHIN_RECT(x1, y1, width, height);
}

bool MOUSE_WITHIN_RECT(int x, int y, int width, int height)
{
    return ((mouse_x >= x && mouse_x < (x + width)) && (mouse_y >= y && mouse_y <= (y + height)));
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
bool GUI_DRAW_BENE_TEXT_MOUSE_SENSITIVE(int x, int y, const std::string& text, int hoverColor) {
    const char *cstring = text.c_str();
    
    alfont_textprintf(bmp_screen, bene_font, x + 1, y + 1, makecol(0, 0, 0), cstring);

	int width = alfont_text_length(bene_font, cstring);
    int height = alfont_text_height(bene_font);
    if (MOUSE_WITHIN_RECT(x, y, width, height)) {
        alfont_textprintf(bmp_screen, bene_font, x, y, hoverColor, cstring);
        return true;
    }
    alfont_textprintf(bmp_screen, bene_font, x, y, makecol(255, 255, 255), cstring);
    return false;	
}

bool GUI_DRAW_FRAME(int x, int y, int width, int height) {
    cRectangle rect = cRectangle(x, y, width, height);
    allegroDrawer->gui_DrawRect(bmp_screen, rect);

//    // fill it up
//    rectfill(bmp_screen, x, y, x+width, y+height, makecol(176,176,196));
//
//    // rect
//    rect(bmp_screen, x,y,x+width, y+height, makecol(252,252,252));
//
//    // lines to darken the right sides
//    line(bmp_screen, x+width, y, x+width , y+height, makecol(84,84,120));
//    line(bmp_screen, x, y+height, x+width , y+height, makecol(84,84,120));

    if ((mouse_x >= x && mouse_x < (x + width)) && (mouse_y >= y && mouse_y <= (y + height))) {
        return true;
    }


    return false; // not hovering on it
}
