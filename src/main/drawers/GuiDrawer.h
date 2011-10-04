/*
 * cGuiDrawer.h
 *
 *  Created on: 27 feb. 2011
 *      Author: Stefan
 */

#ifndef CGUIDRAWER_H_
#define CGUIDRAWER_H_

class GuiDrawer {
	public:
		GuiDrawer();
		virtual ~GuiDrawer();

		void drawShape(GuiShape * shapeToDraw);

	protected:
		void drawRectangle(cRectangle * rect);

	private:
		void drawGuiShape(GuiShape * shape);

		// Button drawing
		void drawButton(GuiButton * guiButton);
		void drawButtonUnpressed(GuiButton * guiButton);
		void drawButtonPressed(GuiButton * guiButton);
		void drawBackground(GuiButton * guiButton);
		void drawLighterBorder(GuiButton * guiButton);
		void drawDarkerBorder(GuiButton * guiButton);

};

#endif /* CGUIDRAWER_H_ */
