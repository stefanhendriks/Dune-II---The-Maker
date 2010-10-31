/*
 * TextDrawer.h
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

#ifndef TEXTDRAWER_H_
#define TEXTDRAWER_H_

class cTextDrawer {
	public:
		cTextDrawer();
		~cTextDrawer();
		void drawTextWithOneInteger(int x, int y, char * msg, int var);
		void drawTextWithTwoIntegers(int x, int y, char * msg, int var1, int var2);
		void drawText(int x, int y, char * msg);

	protected:

	private:

};

#endif /* TEXTDRAWER_H_ */
