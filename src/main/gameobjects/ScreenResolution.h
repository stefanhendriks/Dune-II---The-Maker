#ifndef CSCREENRESOLUTION_H_
#define CSCREENRESOLUTION_H_

class ScreenResolution {

	public:
		ScreenResolution(int theWidth, int theHeight);
		ScreenResolution();
		~ScreenResolution();

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
			return canBeUsed;
		}

		void setTested(bool value) {
			tested = value;
		}
		void setUsable(bool value) {
			canBeUsed = value;
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

		bool canBeUsed;
		bool tested;
};
#endif /* CSCREENRESOLUTION_H_ */
