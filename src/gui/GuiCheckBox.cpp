#include "gui/GuiCheckBox.hpp"

#include "d2tmc.h"
#include "drawers/SDLDrawer.hpp"
#include <iostream>

GuiCheckBox::GuiCheckBox(const cRectangle &rect)
    : GuiObject(rect)
    , m_renderKind(GuiRenderKind::OPAQUE_WITHOUT_BORDER)
    , m_onCheckAction(nullptr)
    , m_onUnCheckAction(nullptr)
    , m_focus(false)
    , m_pressed(false)
    , m_enabled(true)
    , m_checked(true)
{
//    std::cout << "creation\n";
}

GuiCheckBox::~GuiCheckBox()
{
    //std::cout << "destruction\n";
}

void GuiCheckBox::draw() const
{
    switch (m_renderKind) {
        case OPAQUE_WITHOUT_BORDER:
            renderDrawer->renderRectFillColor(m_rect.getX(), m_rect.getY(), m_rect.getWidth(), m_rect.getHeight(), m_theme.fillColor);
            drawBox();
            break;
        case TRANSPARENT_WITHOUT_BORDER:
            drawBox();
            break;
        case OPAQUE_WITH_BORDER:
            renderDrawer->renderRectFillColor(m_rect.getX(), m_rect.getY(), m_rect.getWidth(), m_rect.getHeight(), m_theme.fillColor);
            drawBox();
            break;
        case TRANSPARENT_WITH_BORDER:
            drawRectBorder(m_theme.borderLight, m_theme.borderDark);
            drawBox();
            break;
        case WITH_TEXTURE:
            assert(false);
            break;
    }
}

void GuiCheckBox::drawBox() const
{
    if (m_checked) {
        renderDrawer->renderRectFillColor(m_rect.getX(), m_rect.getY(), m_rect.getWidth(), m_rect.getHeight(), m_theme.borderDark);
        renderDrawer->renderRectColor(m_rect.getX(), m_rect.getY(), m_rect.getWidth(), m_rect.getHeight(), m_theme.borderLight);
    } else {
        renderDrawer->renderRectColor(m_rect.getX(), m_rect.getY(), m_rect.getWidth(), m_rect.getHeight(), m_theme.borderLight);
    }
}

void GuiCheckBox::setRenderKind(GuiRenderKind value)
{
    m_renderKind = value;
}

void GuiCheckBox::nextRenderKind()
{
    if (m_renderKind == GuiRenderKind::OPAQUE_WITH_BORDER) {
        m_renderKind = GuiRenderKind::OPAQUE_WITHOUT_BORDER;
    }
    else if (m_renderKind == GuiRenderKind::OPAQUE_WITHOUT_BORDER) {
        m_renderKind = GuiRenderKind::TRANSPARENT_WITH_BORDER;
    }
    else if (m_renderKind == GuiRenderKind::TRANSPARENT_WITH_BORDER) {
        m_renderKind = GuiRenderKind::TRANSPARENT_WITHOUT_BORDER;
    }
    else if (m_renderKind == GuiRenderKind::TRANSPARENT_WITHOUT_BORDER) {
        m_renderKind = GuiRenderKind::OPAQUE_WITH_BORDER;
    }
}

void GuiCheckBox::onNotifyMouseEvent(const s_MouseEvent &event)
{
    switch (event.eventType) {
        case MOUSE_MOVED_TO:
            onMouseMovedTo(event);
            break;
        case MOUSE_RIGHT_BUTTON_PRESSED:
            onMouseRightButtonPressed(event);
            break;
        case MOUSE_LEFT_BUTTON_CLICKED:
            onMouseLeftButtonClicked(event);
            break;
        case MOUSE_LEFT_BUTTON_PRESSED:
            onMouseLeftButtonPressed(event);
            break;
        default:
            break;
    }
}

void GuiCheckBox::onMouseMovedTo(const s_MouseEvent &event)
{
    m_focus = m_rect.isPointWithin(event.coords);
}

void GuiCheckBox::onMouseRightButtonPressed(const s_MouseEvent &)
{
    if (m_enabled) {
        m_pressed = m_focus;
    }
}

void GuiCheckBox::onMouseLeftButtonPressed(const s_MouseEvent &)
{
    if (m_enabled) {
        m_pressed = m_focus;
    }
}

void GuiCheckBox::onMouseLeftButtonClicked(const s_MouseEvent &)
{
    if (!m_focus || !m_enabled ) {
        //std::cout << "!m_focus || !m_enabled" << std::endl;
        return;
    }
        
    if (m_checked) {
        //std::cout << "to false" << std::endl;
        m_checked = false;
        if (m_onUnCheckAction) {
            m_onUnCheckAction();
        }
    } else {
        m_checked = true;
        //std::cout << "to true" << std::endl;
        if (m_onCheckAction) {
            m_onCheckAction();
        }
    }
}

void GuiCheckBox::setCheckAction(std::function<void()> action)
{
    m_onCheckAction = std::move(action);
}

void GuiCheckBox::setUnCheckAction(std::function<void()> action)
{
    m_onUnCheckAction = std::move(action);
}

void GuiCheckBox::setEnabled(bool value)
{
    m_enabled = value;
}

void GuiCheckBox::setChecked(bool value)
{
    m_checked = value;
}


void GuiCheckBox::onNotifyKeyboardEvent(const cKeyboardEvent &)
{}
