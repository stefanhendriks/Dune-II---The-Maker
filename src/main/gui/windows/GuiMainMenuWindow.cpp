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
	//Logger::getInstance()->debug("Yay we are drawing GuiMainMenuWindow");
	mainMenuDialog->draw();
}
