#include "cGuiWindow.h"

#include "d2tmc.h"
#include "drawers/cAllegroDrawer.h"

#include <allegro/color.h>

cGuiWindow::cGuiWindow(const cRectangle& rect) : cGuiObject(rect) {
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
    allegroDrawer->gui_DrawRect(bmp_screen, m_rect);

    for (auto & guiObject : gui_objects) {
        guiObject->draw();
    }

    // draw title
    textDrawer.drawTextCentered(title.c_str(), m_rect.getX(), m_rect.getWidth(), m_rect.getY() + 2, colorYellow);
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
    return cRectangle(x + m_rect.getX(), y + m_rect.getY(), width, height);
}

void cGuiWindow::onNotifyKeyboardEvent(const cKeyboardEvent &) {
}
