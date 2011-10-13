#ifndef GUICOLORS_H_
#define GUICOLORS_H_

#include "allegro.h"

class GuiColors {
	public:
		GuiColors() {};
		~GuiColors() {};

		int getMenuGreyColor() { return makecol(176, 176, 196); }
		int getMenuLightBorderColor() { return makecol(252, 252, 252); }
		int getMenuDarkBorderColor() { return makecol(84, 84, 120); }

};

#endif /* GUICOLORS_H_ */
