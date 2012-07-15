#ifndef GUICOLORS_H_
#define GUICOLORS_H_

#include "allegro.h"

class GuiColors {
	public:
		GuiColors() {};
		~GuiColors() {};

		int getMenuGrey() { return makecol(176, 176, 196); }
		int getMenuLightBorder() { return makecol(252, 252, 252); }
		int getMenuDarkBorder() { return makecol(84, 84, 120); }
		int getMenuYellow() { return makecol(214, 149, 0); }

		int getWhite() { return makecol(255, 255, 255); }
		int getBlack() { return makecol(0,0,0); }
		int getDarkGrey() { return makecol(64, 64, 64); }
		int getGrey() { return makecol(128, 128, 128); }

		int getRed() { return makecol(255, 0, 0); }
		int getGreen() { return makecol(0, 255, 0); }
		int getBlue() { return  makecol(0, 0, 255); }
};

#endif /* GUICOLORS_H_ */
