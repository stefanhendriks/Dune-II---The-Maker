#include "GuiWindow.h"

#include "d2tmc.h"
#include "drawers/SDLDrawer.hpp"


GuiWindow::GuiWindow(const cRectangle &rect, cTextDrawer* _textDrawer ) : GuiObject(rect)
{
    this->title = "";
    gui_objects = std::vector<GuiObject *>(0);
    m_textDrawer = _textDrawer;
    m_textDrawer->setTextColor(Color::yellow());
}

GuiWindow::~GuiWindow() noexcept
{
    for (auto gui_object : gui_objects) {
        delete gui_object;
    }
}

void GuiWindow::draw() const
{
    // draw window itself...
    drawRectFillBorder(m_theme);
    for (auto &guiObject : gui_objects) {
        guiObject->draw();
    }

    // draw title
    m_textDrawer->drawTextCentered(title.c_str(), m_rect.getX(), m_rect.getWidth(), m_rect.getY() + 2, Color::yellow());
}

void GuiWindow::addGuiObject(GuiObject *guiObject)
{
    gui_objects.push_back(guiObject);
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

void GuiWindow::onNotifyKeyboardEvent(const cKeyboardEvent &)
{
}
