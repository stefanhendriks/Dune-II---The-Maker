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
		cGuiShape();
		cGuiShape(int theX, int theY, int theHeight, int theWidth);

		~cGuiShape();

		void setX(int value) {
			x = value;
		}
		void setY(int value) {
			y = value;
		}
		void setHeight(int value) {
			height = value;
		}
		void setWidth(int value) {
			width = value;
		}

		int getX() {
			return x;
		}
		int getY() {
			return y;
		}
		int getHeight() {
			return height;
		}
		int getWidth() {
			return width;
		}

		bool isMouseOverShape();

	protected:

	private:
		int x, y, height, width;
};

#endif /* SHAPE_H_ */
