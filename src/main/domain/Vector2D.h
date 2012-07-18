#pragma once
#ifndef VECTOR_2D_H
#define VECTOR_2D_H

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

		int getX() { 
			return x; 
		}

		int getY() {
			return y;
		}

	private:
		int x, y;

};

#endif