#include "gui/GuiBar.h"

#include "d2tmc.h"
#include "drawers/SDLDrawer.hpp"


GuiBar::GuiBar(const cRectangle &rect, GuiBarPlacement placement) :
    GuiObject(rect), gui_objects(std::vector<GuiObject *>(0)), m_placement(placement), placementPosition(0)
{
}

GuiBar::~GuiBar() noexcept
{
    for (auto gui_object : gui_objects) {
        delete gui_object;
    }
}

void GuiBar::draw() const
{
    // draw window itself...
    //drawRectFillBorder(m_theme);
    //drawRectBorder(m_theme.borderLight,m_theme.borderLight);
    drawRectFillBorder(m_theme);
    for (auto &guiObject : gui_objects) {
        guiObject->draw();
    }

}

void GuiBar::addGuiObject(GuiObject *guiObject)
{
    gui_objects.push_back(guiObject);
}


void GuiBar::onNotifyMouseEvent(const s_MouseEvent &event)
{
    if (!event.coords.isWithinRectangle(&m_rect))
        return;
    for (auto &guiObject : gui_objects) {
        guiObject->onNotifyMouseEvent(event);
    }
}

cRectangle GuiBar::getRelativeRect(int x, int y, int width, int height)
{
    return cRectangle(x + m_rect.getX(), y + m_rect.getY(), width, height);
}

void GuiBar::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    for (auto &guiObject : gui_objects) {
        guiObject->onNotifyKeyboardEvent(event);
    }
}
