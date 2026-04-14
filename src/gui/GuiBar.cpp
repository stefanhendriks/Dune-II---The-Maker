#include "gui/GuiBar.h"

#include "drawers/SDLDrawer.hpp"
#include <cassert>

GuiBar::GuiBar(SDLDrawer* drawer, const cRectangle &rect, GuiBarPlacement placement, int heightBarSize) :
    GuiObject(drawer, rect), /*gui_objects(std::vector<GuiObject *>(0)),*/ m_placement(placement), placementPosition(0), heightBarSize(heightBarSize)
{
    assert(drawer != nullptr);
    if (m_placement == GuiBarPlacement::HORIZONTAL) {
       halfMarginBetweenButtons = (rect.getHeight()-heightBarSize)/2;
    } else {
        halfMarginBetweenButtons = (rect.getWidth()-heightBarSize)/2;
    }
}

GuiBar::~GuiBar() noexcept
{
    // Smart pointers handle cleanup
    // for (auto gui_object : gui_objects) {
    //     delete gui_object;
    // }
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

void GuiBar::addGuiObject(std::unique_ptr<GuiObject> guiObject)
{
    gui_objects.push_back(std::move(guiObject));
}

void GuiBar::addAutoGuiObject(std::unique_ptr<GuiObject> guiObject)
{
    int x = 0, y = 0;
    if (m_placement == GuiBarPlacement::HORIZONTAL) {
        x = placementPosition;
        y = halfMarginBetweenButtons;
        placementPosition += heightBarSize + 2*halfMarginBetweenButtons;
    } else {
        x = m_rect.getX() + halfMarginBetweenButtons;
        y = m_rect.getY() + placementPosition;
        placementPosition += heightBarSize + 2*halfMarginBetweenButtons;
    }
    guiObject->setPosition(x,y);
    gui_objects.push_back(std::move(guiObject));
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
