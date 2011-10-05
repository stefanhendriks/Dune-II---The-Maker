/*
 * cGuiMainMenuWindow.h
 *
 *  Created on: 4 okt. 2011
 *      Author: Stefan
 */

#ifndef CGUIMAINMENUWINDOW_H_
#define CGUIMAINMENUWINDOW_H_

#include "GuiWindow.h"
#include "GuiMainMenuWindowInteractionDelegate.h"

class GuiMainMenuWindow : public GuiWindow {
	public:
		GuiMainMenuWindow(GuiMainMenuWindowInteractionDelegate * mainMenuWindowDelegate);
		~GuiMainMenuWindow();

	private:

};

#endif /* CGUIMAINMENUWINDOW_H_ */
