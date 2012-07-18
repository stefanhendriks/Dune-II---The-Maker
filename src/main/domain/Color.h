#pragma once
#ifndef COLOR_H
#define COLOR_H

#include "../include/allegro.h"

class Color {

	public:
		Color(int red, int green, int blue) {
			this->red = red;
			this->green = green;
			this->blue = blue;
		}

		int makeColor() {
			return makecol(red, green, blue);
		}
		

	private:
		int red, green, blue;

};

namespace Colors {

	static Color Magenta = Color(255, 0, 255);
	static Color Blue = Color(0, 0, 255);
	static Color White = Color(255, 255, 255);
	static Color Black = Color(0, 0, 0);
	static Color Red = Color(255, 0, 0);
	static Color Green = Color(0, 255, 0);

}

#endif