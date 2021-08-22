/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2021 (c) code by Stefan Hendriks

  */

// GUI ROUTINES

#ifndef GUI_H
#define GUI_H

struct sButton
{
    int iX, iY;
    int iWidth, iHeight;
    bool bHover; //  hovers over this thing.
};

bool GUI_DRAW_FRAME(int x1, int y1, int width, int height);
bool GUI_DRAW_FRAME_PRESSED(int x1, int y1, int width, int height);
void GUI_DRAW_BENE_TEXT(int x, int y, const std::string& text);
bool MOUSE_WITHIN_RECT(int x, int y, int width, int height);
bool GUI_DRAW_BENE_TEXT_MOUSE_SENSITIVE(int x, int y, const std::string& text, int hoverColor);
bool GUI_DRAW_FRAME_WITH_COLORS(int x1, int y1, int width, int height, int borderColor, int fillColor);

#endif // GUI_H
