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

		// Button drawing
		void drawButton(cGuiButton * guiButton);
		void drawButtonUnpressed(cGuiButton * guiButton);
		void drawButtonPressed(cGuiButton * guiButton);
		void drawBackground(cGuiButton * guiButton);

};

#endif /* CGUIDRAWER_H_ */
