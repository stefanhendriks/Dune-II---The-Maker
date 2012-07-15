#include "GuiWindow.h"

#include "../../utils/Logger.h"

GuiWindow::~GuiWindow() {
	delete interactionDelegate;
	Logger::getInstance()->debug("destructor GuiWindow");
}

void GuiWindow::draw() {
	// derived classes implement this
}
