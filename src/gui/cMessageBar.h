/*
 * cMessageBar.h
 *
 *  Created on: 9-nov-2010
 *      Author: Stefan
 *
 *  A message bar is a yellow 'bar'. It has a single line drawn within, and has a fixed height.
 *  It is used for drawing the amount a unit/structure costs, when hovering above a buildingListItem
 */

#ifndef CMESSAGEBAR_H_
#define CMESSAGEBAR_H_

class cMessageBar {
	public:
		cMessageBar(int theX, int theY, int theWidth);
		~cMessageBar();

		void setX(int value) {
			x = value;
		}
		void setY(int value) {
			y = value;
		}
		void setMessage(std::string * value) {
			message = value;
		}
		void setAlpha(int value) {
			alpha = value;
		}
		void setWidth(int value) {
			width = value;
		}

		int getX() {
			return x;
		}
		int getAlpha() {
			return alpha;
		}
		int getY() {
			return y;
		}
		int getWidth() {
			return width;
		}
		std::string * getMessage() {
			return message;
		}

	protected:

	private:
		int alpha;
		int width;
		int x;
		int y;
		std::string * message;

};

#endif /* CMESSAGEBAR_H_ */
