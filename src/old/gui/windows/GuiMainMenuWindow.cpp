#include "GuiMainMenuWindow.h"

#include "../../utils/Logger.h"

GuiMainMenuWindow::GuiMainMenuWindow(GuiMainMenuWindowInteractionDelegate * mainMenuWindowDelegate) : GuiWindow((GuiInteractionDelegate *)mainMenuWindowDelegate) {
	mainMenuDialog = NULL;
}

GuiMainMenuWindow::~GuiMainMenuWindow() {
	delete mainMenuDialog;
	Logger::getInstance()->debug("destructor GuiMainMenuWindow");
}

void GuiMainMenuWindow::draw() {
	mainMenuDialog->draw();
}
