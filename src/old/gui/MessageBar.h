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

class MessageBar {
	public:
		MessageBar(int theX, int theY, int theWidth);
		~MessageBar();

		void setX(int value) {
			x = value;
		}
		void setY(int value) {
			y = value;
		}

		void clearMessage() {
			memset(cMessage, 0, sizeof(cMessage));
		}

		void setMessage(const char msg[255]) {
			resetTimerMessage();
			clearMessage();
			sprintf(cMessage, "%s", msg);
		}

		void setAlpha(int value) {
			alpha = value;
		}

		void incrementAlpha() {
			alpha += 3;
			if (alpha > 254) {
				alpha = 254;
			}
		}

		void decreaseAlpha() {
			alpha -= 6;
			if (alpha < 0) {
				alpha = 0;
			}
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

		char * getMessage() {
			return &cMessage[0];
		}

		bool isMessageSet() {
			return cMessage[0] != '\0';
		}

		void incrementTimerMessage() {
			TIMER_message++;
		}

		void resetTimerMessage() {
			TIMER_message = 0;
		}

		int getTimerMessage() {
			return TIMER_message;
		}

	protected:

	private:
		int alpha;
		int width;
		int x;
		int y;

		char cMessage[255];
		int TIMER_message;
};

#endif /* CMESSAGEBAR_H_ */
