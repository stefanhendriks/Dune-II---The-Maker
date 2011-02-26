/*
 * Rectangle.h
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

#ifndef SHAPE_H_
#define SHAPE_H_

class Shape {
	public:
		Shape(int theX, int theY, int theHeight, int theWidth);
		Shape();
		~Shape();

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

	protected:

	private:
		int x, y, height, width;
};

#endif /* SHAPE_H_ */
