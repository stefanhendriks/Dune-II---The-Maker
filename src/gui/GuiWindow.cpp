#include "GuiWindow.h"
#include "drawers/cTextDrawer.h"
#include "drawers/SDLDrawer.hpp"
#include <cassert>

GuiWindow::GuiWindow(SDLDrawer* drawer, const cRectangle &rect, cTextDrawer* _textDrawer) :
    GuiObject(drawer,rect),
    gui_objects(),
    title(""),
    m_textDrawer(_textDrawer)
{
    assert(drawer != nullptr);
    assert(_textDrawer != nullptr);
}

GuiWindow::~GuiWindow() noexcept
{
    // Smart pointers handle cleanup
}

void GuiWindow::draw() const
{
    // draw window itself...
    drawRectFillBorder(m_theme);
    for (auto &guiObject : gui_objects) {
        guiObject->draw();
    }

    // draw title
    m_textDrawer->drawTextCentered(title.c_str(), m_rect.getX(), m_rect.getWidth(), m_rect.getY() + 2, Color::Yellow);
}

void GuiWindow::addGuiObject(std::unique_ptr<GuiObject> guiObject)
{
    gui_objects.push_back(std::move(guiObject));
}


void GuiWindow::onNotifyMouseEvent(const s_MouseEvent &event)
{
    for (auto &guiObject : gui_objects) {
        guiObject->onNotifyMouseEvent(event);
    }
}

cRectangle GuiWindow::getRelativeRect(int x, int y, int width, int height)
{
    return cRectangle(x + m_rect.getX(), y + m_rect.getY(), width, height);
}

void GuiWindow::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (event.isType(eKeyEventType::PRESSED)) {
        for (auto &guiObject : gui_objects) {
            guiObject->onNotifyKeyboardEvent(event);
        }
    }
}
