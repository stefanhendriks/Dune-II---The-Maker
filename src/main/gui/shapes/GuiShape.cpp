#include "../../include/d2tmh.h"

GuiShape::GuiShape(cRectangle * rect) {
	assert(rect);
	rectangle = rect;
	rectangleOwner = false;
	setColors(makecol(32, 32, 32), makecol(255,255,255), makecol(128,128,128));
}

GuiShape::GuiShape(int x, int y, int width, int height) {
	int x2 = (x + width);
	int y2 = (y + height);
	rectangle = new cRectangle(x, y, x2, y2);
	rectangleOwner = true;
	setColors(makecol(32, 32, 32), makecol(255,255,255), makecol(128,128,128));
}


GuiShape::~GuiShape() {
	// this boolean holds if we (this class) created an instance, and thus is responsible
	// for deleting it. In other cases, the cRectangle is created outside this class
	// and the responsibility to delete it lays also outside this class!
	if (rectangleOwner) {
		if (rectangle) {
			delete rectangle;
		}
		rectangle = NULL;
	}
}

bool GuiShape::isMouseOverShape() {
	cMouse * mouse = cMouse::getInstance();
	return mouse->isOverRectangle(rectangle);
}

void GuiShape::setColors(int darkBorder, int lightBorder, int inner) {
	darkBorderColor = darkBorder;
	lightBorderColor = lightBorder;
	innerColor = inner;
}
