#pragma once
#ifndef CMOUSE_H_
#define CMOUSE_H_

#include "../math/Vector2D.h"
#include "Bitmap.h"

class Mouse {

	public:
		Mouse(Bitmap * mouseBitmap);
		~Mouse();

		void updateState();

		int getX() { return x; }
		int getY() { return y; }
		int getZ() { return z; }

		Vector2D getVector2D() {
			Vector2D v;
			v.set(x, y);
			return v;
		}

		Bitmap * getBitmap() {
			return mouseBitmap;
		}

	private:
		static Mouse *instance;
		int x, y, z;

		Bitmap * mouseBitmap;
};

#endif