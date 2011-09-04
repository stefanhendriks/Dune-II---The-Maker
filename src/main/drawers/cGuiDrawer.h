/*
 * cGuiDrawer.h
 *
 *  Created on: 27 feb. 2011
 *      Author: Stefan
 */

#ifndef CGUIDRAWER_H_
#define CGUIDRAWER_H_

class cGuiDrawer {
	public:
		cGuiDrawer();
		virtual ~cGuiDrawer();

		void drawShape(cGuiShape * shapeToDraw);

	protected:
		void drawRectangle(cRectangle * rect);

	private:
		void drawGuiShape(cGuiShape * shape);
		void drawButton(cGuiButton * guiButton);
};

#endif /* CGUIDRAWER_H_ */
