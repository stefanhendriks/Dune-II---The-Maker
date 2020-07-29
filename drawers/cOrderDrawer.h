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

		bool isMouseOverOrderButton(int mouseX, int mouseY);

	protected:

	private:

};

#endif /* CORDERDRAWER_H_ */
