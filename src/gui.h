/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

// GUI ROUTINES

#pragma once

#include <string>

bool GUI_DRAW_FRAME(int x, int y, int width, int height);
bool GUI_DRAW_FRAME_PRESSED(int x1, int y1, int width, int height);
bool MOUSE_WITHIN_RECT(int x, int y, int width, int height);
bool GUI_DRAW_BENE_TEXT_MOUSE_SENSITIVE(int x, int y, const std::string &text, int hoverColor);
