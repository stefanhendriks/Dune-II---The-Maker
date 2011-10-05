#include "GuiMainMenuWindow.h"

GuiMainMenuWindow::GuiMainMenuWindow(GuiMainMenuWindowInteractionDelegate * mainMenuWindowDelegate) : GuiWindow((GuiInteractionDelegate *)mainMenuWindowDelegate) {
	mainMenuDialog = NULL;
}

GuiMainMenuWindow::~GuiMainMenuWindow() {
	delete mainMenuDialog;
}

void GuiMainMenuWindow::draw() {
}
