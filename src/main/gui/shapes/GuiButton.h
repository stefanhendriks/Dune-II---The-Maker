/**
 * A button is a gui shape (rectangle), with a label.
 *
 */
#ifndef CGUIBUTTON_H_
#define CGUIBUTTON_H_

#include <sstream>
#include <string>

#include "allegro.h"

#include "../../gameobjects/cRectangle.h"
#include "GuiShape.h"

class GuiButton : public GuiShape {

	public:
		GuiButton(cRectangle * rect, std::string theLabel);
		GuiButton(int x, int y, int width, int height, std::string theLabel);
		~GuiButton();

		void draw();

		std::string * getLabel() {
			return label;
		}

		BITMAP * getBitmap() { return bitmap; }
		void setBitmap(BITMAP * value) { bitmap = value; }
		void setHasBorders(bool value) { hasBorders = value; }
		void setPressed(bool value) { pressed = value; }

		bool shouldDrawPressedWhenMouseHovers() { return drawPressedWhenMouseHovers; }
		bool shouldDrawBorders() { return hasBorders; }
		void setDrawPressedWhenMouseHovers(bool value) { drawPressedWhenMouseHovers = value; }
		bool isPressed() { return pressed; }

	private:
		std::string * label;
		bool drawPressedWhenMouseHovers;
		bool hasBorders;
		bool pressed;
		BITMAP * bitmap;

		void drawBackground();
		void drawLighterBorder();
		void drawDarkerBorder();
		void drawButtonUnpressed();
		void drawButtonPressed();
};

#endif /* CGUIBUTTON_H_ */
