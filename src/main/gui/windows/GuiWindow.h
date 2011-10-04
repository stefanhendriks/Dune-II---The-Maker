/*
 * GuiWindow.h
 *
 *  Created on: 4 okt. 2011
 *      Author: Stefan
 */

/**
 * A gui window is not the same as a dialog or a shape. It functions as a container for shapes and dialogs. Like, the main menu
 * is a gui window. A gui window has a one on one relation with a state.
 *
 * Concrete implementations of windows:
 * - main menu window
 * - select next conquest window
 * - mentat window
 * - select your next conquest window
 * - in combat window
 *
 * A cGuiWindow is drawn by a drawer. The interact function will delegate any interactions to an interact class, seperating
 * view from model.
 *
 */

#ifndef CGUIWINDOW_H_
#define CGUIWINDOW_H_

#include <assert.h>
#include <cstddef>

#include "../GuiElement.h"
#include "../GuiDelegate.h"

class GuiWindow : public GuiElement {

	public:
		GuiWindow(GuiDelegate * guiDelegate) {
			assert(guiDelegate);
			delegate = guiDelegate;
		}

		~GuiWindow() {
			delegate = NULL;
		}

		// called each frame, delegates its behavior to the given concrete cGuiWindowDelegate class.
		void interact() {
			delegate->interact(this);
		}

	private:
		GuiDelegate * delegate;

};

#endif /* CGUIWINDOW_H_ */
