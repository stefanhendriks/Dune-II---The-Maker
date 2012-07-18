#pragma once
#ifndef VECTOR_2D_H
#define VECTOR_2D_H

#include <stdio.h>
#include "../utils/Logger.h"
#include "Rectangle.h"

class Vector2D {

	public:
		Vector2D() {
			x = y = 0;
		}

		Vector2D(int x, int y) {
			this->x = x;
			this->y = y;
		}

		void set(int x, int y) {
			this->x = x;
			this->y = y;
		}

		void set(Vector2D &other) {
			this->x = other.x;
			this->y = other.y;
		}

		void addX(int i) {
			x += i;
		}

		void addY(int i) {
			y += i;
		}

		void substractX(int i) {
			x -= i;
		}

		void substractY(int i) {
			y -= i;
		}

		void clipX(int minX, int maxX) {
			x = clipValue(x, minX, maxX);
		}

		void clipY(int minY, int maxY) {
			y = clipValue(y, minY, maxY);
		}

		void clip(int minX, int minY, int maxX, int maxY) {
			clipX(minX, maxX);
			clipY(minY, maxY);
		}

		void clip(Rectangle &rectangle) {
			int highestX = rectangle.getHighestX();
			int highestY = rectangle.getHighestY();
			int lowestX = rectangle.getLowestX();
			int lowestY = rectangle.getLowestY();
			clip(lowestX, lowestY, highestX, highestY);
		}

		int getX() { 
			return x; 
		}

		int getY() {
			return y;
		}

	protected:
		int clipValue(int value, int min, int max) {
			if (value < min) return min;
			if (value > max) return max;
			return value;
		}

	private:
		int x, y;

};

namespace Vector2Ds {
	static Vector2D ZeroDotZero = Vector2D(0, 0);
}

#endif