#include "allegro.h"

#include "GuiButton.h"

GuiButton::GuiButton(cRectangle * rect, std::string theLabel) : GuiShape(rect) {
	drawPressedWhenMouseHovers = false;
	bitmap = NULL;
	label = &theLabel;
	hasBorders = true;
}

GuiButton::GuiButton(int x, int y, int width, int height, std::string theLabel) : GuiShape(x, y, width, height){
	label = &theLabel;
	hasBorders = true;
	drawPressedWhenMouseHovers = false;
}

GuiButton::~GuiButton() {
	drawPressedWhenMouseHovers = false;
	bitmap = NULL;
}
