/**
 * A gui shape represents a thing, a shape for better naming, in the gui. In this world those shapes are mostly rectangular and
 * therefore a cGuiShape uses a rectangle to represents its position and its size.
 *
 * Using this as a base class a cGuiButton and even a dialog can extend from this. Often gui elements within dune 2 have very
 * simple border (in order to draw).
 *
 */
#ifndef GUISHAPE_H_
#define GUISHAPE_H_

#include "../GuiElement.h"
#include "../../gameobjects/Rectangle.h"
#include "../GuiColors.h"

class GuiShape : public GuiElement {

	public:
		GuiShape(Rectangle * rect);
		GuiShape(int x, int y, int width, int height);

		virtual ~GuiShape();

		void draw();

		void setX(int value) { rectangle->setStartX(value); 	}
		void setY(int value) { rectangle->setStartY(value);		}

		Rectangle * getRectangle() { return rectangle; }

		bool isMouseOverShape();

		void setColors(int darkBorder, int lightBorder, int inner);

		int getDarkBorderColor() { return darkBorderColor; }
		int getLightBorderColor() { return lightBorderColor; }
		int getInnerColor() { return innerColor; }

	protected:
		Rectangle * rectangle;

	private:
		GuiColors guiColors;
		bool rectangleOwner;
		int darkBorderColor;
		int lightBorderColor;
		int innerColor;
};

#endif /* SHAPE_H_ */
