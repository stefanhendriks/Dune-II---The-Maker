/*
 * cOrderDrawer.h
 *
 *  Created on: 9-aug-2010
 *      Author: Stefan
 */

#ifndef CORDERDRAWER_H_
#define CORDERDRAWER_H_

class cOrderDrawer {
	public:
		cOrderDrawer();
		~cOrderDrawer();

		void drawOrderButton(const cPlayer & thePlayer);
		void drawOrderPlaced(const cPlayer & thePlayer);

		bool isMouseOverOrderButton();

    void drawRectangleOrderButton();

protected:

	private:
        cRectangle * buttonRect;
        BITMAP * buttonBitmap;

};

#endif /* CORDERDRAWER_H_ */
