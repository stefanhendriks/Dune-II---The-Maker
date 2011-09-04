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
		cScreenResolution();
		~cScreenResolution();

		int getWidth() {
			return width;
		}
		int getHeight() {
			return height;
		}

		bool isTested() {
			return tested;
		}
		bool isUsable() {
			return usable;
		}

		void setTested(bool value) {
			tested = value;
		}
		void setUsable(bool value) {
			usable = value;
		}

		void setWidth(int value) {
			width = value;
		}
		void setHeight(int value) {
			height = value;
		}

		void printLog();

	private:
		int width, height;

		bool usable; // can this resolution be used?
		bool tested;
};
#endif /* CSCREENRESOLUTION_H_ */
