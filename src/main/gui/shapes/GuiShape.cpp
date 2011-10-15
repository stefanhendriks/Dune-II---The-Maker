#include "../../include/d2tmh.h"

GuiShape::GuiShape(Rectangle * rect) {
	assert(rect);
	rectangle = rect;
	rectangleOwner = false;
	setColors(makecol(32, 32, 32), makecol(255,255,255), makecol(128,128,128));
}

GuiShape::GuiShape(int x, int y, int width, int height) {
	int x2 = (x + width);
	int y2 = (y + height);
	rectangle = new Rectangle(x, y, x2, y2);
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
	Mouse * mouse = Mouse::getInstance();
	return mouse->isOverRectangle(rectangle);
}

void GuiShape::setColors(int darkBorder, int lightBorder, int inner) {
	darkBorderColor = darkBorder;
	lightBorderColor = lightBorder;
	innerColor = inner;
}

void GuiShape::draw() {
	int x = rectangle->getStartX();
	int y = rectangle->getStartY();
	int width = rectangle->getWidth();
	int height = rectangle->getHeight();

	int lightColor = getLightBorderColor();
	int darkColor = getDarkBorderColor();
	int inner = getInnerColor();

	rectfill(bmp_screen, x, y, x + width, y + height, inner);
	rect(bmp_screen, x, y, x + width, y + height, lightColor);

	// lines to darken the right sides
	line(bmp_screen, x + width, y, x + width, y + height, darkColor);
	line(bmp_screen, x, y + height, x + width, y + height, darkColor);
}
