#include "d2tmh.h"

cGuiWindow::cGuiWindow(cRectangle rect) {
    this->rect = rect;
    gui_objects = std::vector<cGuiObject*>(0);
}

cGuiWindow::~cGuiWindow() noexcept {
    for (int i = 0; i < gui_objects.size(); i++) {
        delete gui_objects[i];
    }
}

void cGuiWindow::draw() const {
    // draw window itself...
    allegroDrawer->gui_DrawRect(bmp_screen, rect);

    for (auto & guiObject : gui_objects) {
        guiObject->draw();
    }
}

void cGuiWindow::addGuiObject(cGuiObject *guiObject) {
    gui_objects.push_back(guiObject);
}

void cGuiWindow::onNotifyMouseEvent(const s_MouseEvent &event) {
    for (auto & guiObject : gui_objects) {
        guiObject->onNotifyMouseEvent(event);
    }
}
