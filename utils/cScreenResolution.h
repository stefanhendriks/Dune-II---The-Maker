/*
 * cScreenResolution.h
 *
 *  Created on: 24 dec. 2010
 *      Author: Stefan
 */

#ifndef CSCREENRESOLUTION_H_
#define CSCREENRESOLUTION_H_

class cScreenResolution {

	public:
		cScreenResolution(int theWidth, int theHeight);
		~cScreenResolution();

		int getWidth() { return width; }
		int getHeight() { return height; }

	private:
		int width, height;
};
#endif /* CSCREENRESOLUTION_H_ */
