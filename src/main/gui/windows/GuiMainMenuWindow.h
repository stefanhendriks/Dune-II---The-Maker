/*
 * cGuiMainMenuWindow.h
 *
 *  Created on: 4 okt. 2011
 *      Author: Stefan
 */

#ifndef CGUIMAINMENUWINDOW_H_
#define CGUIMAINMENUWINDOW_H_

#include "GuiWindow.h"
#include "GuiMainMenuWindowDelegate.h"

class GuiMainMenuWindow : public GuiWindow {
	public:
		GuiMainMenuWindow(GuiMainMenuWindowDelegate * mainMenuWindowDelegate);
		~GuiMainMenuWindow();

	private:

};

#endif /* CGUIMAINMENUWINDOW_H_ */
