#pragma once
#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "Screen.h"

class Viewport {

	public:
		Viewport(int width, int height) {
			this->width = width;
			this->height = height;
			this->bitmap = new Bitmap(width, height);
		}
	
		void draw(Screen * screen, int x, int y);

	private:
		int width, height;

		Bitmap * bitmap;
};

#endif
