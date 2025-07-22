#include "GuiWindow.hpp"

#include "d2tmc.h"
#include "drawers/SDLDrawer.hpp"


GuiWindow::GuiWindow(const cRectangle &rect) : GuiObject(rect)
{
    this->title = "";
    this->textDrawer = cTextDrawer(bene_font);
    textDrawer.setTextColor(Color::yellow());
}

GuiWindow::~GuiWindow() noexcept
{
}

void GuiWindow::draw() const
{
    drawRectFillBorder(m_theme);

    for (auto &guiObject : gui_objects) {
        guiObject->draw();
    }
    // draw title
    textDrawer.drawTextCentered(title.c_str(), m_rect.getX(), m_rect.getWidth(), m_rect.getY() + 2, Color::yellow());
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

void GuiWindow::onNotifyKeyboardEvent(const cKeyboardEvent &)
{
}
