/*
 * Rectangle.h
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

#ifndef GUISHAPE_H_
#define GUISHAPE_H_

class cGuiShape {

	public:
		cGuiShape(cRectangle * rect);
		cGuiShape(int x, int y, int width, int height);

		virtual ~cGuiShape();

		void setX(int value) { rectangle->setStartX(value); 	}
		void setY(int value) { rectangle->setStartY(value);		}

		cRectangle * getRectangle() { return rectangle; }

		bool isMouseOverShape();

		void setColors(int darkBorder, int lightBorder, int inner);

		int getDarkBorderColor() { return darkBorderColor; }
		int getLightBorderColor() { return lightBorderColor; }
		int getInnerColor() { return innerColor; }

	protected:

	private:
		cRectangle * rectangle;
		bool rectangleOwner;
		int darkBorderColor;
		int lightBorderColor;
		int innerColor;
};

#endif /* SHAPE_H_ */
