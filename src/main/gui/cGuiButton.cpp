#include "allegro.h"

#include <sstream>
#include <string>

#include "../gameobjects/cRectangle.h"
#include "cGuiShape.h"
#include "cGuiButton.h"

cGuiButton::cGuiButton(cRectangle * rect, std::string theLabel) : cGuiShape(rect) {
	drawPressedWhenMouseHovers = false;
	bitmap = NULL;
	label = &theLabel;
}

cGuiButton::cGuiButton(int x, int y, int width, int height, std::string theLabel) : cGuiShape(x, y, width, height){
	label = &theLabel;
}

cGuiButton::~cGuiButton() {
	drawPressedWhenMouseHovers = false;
	bitmap = NULL;
}
