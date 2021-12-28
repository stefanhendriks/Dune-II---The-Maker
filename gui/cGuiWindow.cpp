#include "d2tmh.h"
#include "cGuiWindow.h"


cGuiWindow::cGuiWindow(cRectangle rect) {
    this->rect = rect;
    this->title = "";
    gui_objects = std::vector<cGuiObject*>(0);
    this->textDrawer = cTextDrawer(bene_font);
    int colorYellow = makecol(255, 207, 41);
    textDrawer.setTextColor(colorYellow);
}

cGuiWindow::~cGuiWindow() noexcept {
    for (auto gui_object : gui_objects) {
        delete gui_object;
    }
}

void cGuiWindow::draw() const {
    int colorYellow = makecol(255, 207, 41);
    // draw window itself...
    allegroDrawer->gui_DrawRect(bmp_screen, rect);

    for (auto & guiObject : gui_objects) {
        guiObject->draw();
    }

    // draw title
    textDrawer.drawTextCentered(title.c_str(), rect.getX(), rect.getWidth(), rect.getY() + 2, colorYellow);
}

void cGuiWindow::addGuiObject(cGuiObject *guiObject) {
    gui_objects.push_back(guiObject);
}


void cGuiWindow::onNotifyMouseEvent(const s_MouseEvent &event) {
    for (auto & guiObject : gui_objects) {
        guiObject->onNotifyMouseEvent(event);
    }
}

cRectangle cGuiWindow::getRelativeRect(int x, int y, int width, int height) {
    cRectangle result = cRectangle(x + rect.getX(), y + rect.getY(), width, height);
    return result;
}
