#include "GuiButton.h"

#include "game/cGame.h"
#include "include/d2tmc.h"
#include "drawers/SDLDrawer.hpp"
#include "drawers/cTextDrawer.h"
#include "game/cGameSettings.h"
#include <cassert>

GuiButton::GuiButton(SDLDrawer* drawer, const cRectangle &rect, const std::string &btnText)
    : GuiObject(drawer, rect)
    , m_textDrawer(nullptr)
    , m_buttonText(btnText)
    , m_renderKind(GuiRenderKind::OPAQUE_WITHOUT_BORDER)
    , m_textAlignHorizontal(GuiTextAlignHorizontal::CENTER)
    , m_onLeftMouseButtonClickedAction(nullptr)
    , m_onRightMouseButtonClickedAction(nullptr)
    , m_tex(nullptr)
    , m_focus(false)
    , m_pressed(false)
    , m_enabled(true)   // by default always enabled
{
    assert(drawer != nullptr);
}

GuiButton::~GuiButton()
{
}

void GuiButton::draw() const
{
    switch (m_renderKind) {
        case OPAQUE_WITHOUT_BORDER:
            m_renderDrawer->renderRectFillColor(m_rect.getX(), m_rect.getY(), m_rect.getWidth(), m_rect.getHeight(), m_theme.fillColor);
            drawText();
            break;
        case TRANSPARENT_WITHOUT_BORDER:
            drawText();
            break;
        case OPAQUE_WITH_BORDER:
            m_renderDrawer->renderRectFillColor(m_rect.getX(), m_rect.getY(), m_rect.getWidth(), m_rect.getHeight(), m_enabled ? m_theme.fillColor : m_theme.disabledFillColor);
            if (m_pressed) {
                m_renderDrawer->gui_DrawRectBorder(m_rect, m_theme.borderDark, m_theme.borderLight);
            }
            else {
                if (m_enabled) {
                    m_renderDrawer->gui_DrawRectBorder(m_rect, m_theme.borderLight, m_theme.borderDark);
                } else {
                    m_renderDrawer->gui_DrawRectBorder(m_rect, m_theme.disabledBorderLight, m_theme.disabledBorderDark);
                }
            }
            drawText();
            break;
        case TRANSPARENT_WITH_BORDER:
            if (m_pressed) {
                m_renderDrawer->gui_DrawRectBorder(m_rect, m_theme.borderDark, m_theme.borderLight);
            }
            else {
                m_renderDrawer->gui_DrawRectBorder(m_rect, m_theme.borderLight, m_theme.borderDark);
            }
            drawText();
            break;
        case WITH_TEXTURE:
            m_renderDrawer->renderSprite(m_tex, m_rect.getX(),m_rect.getY());
            break;
        case WITH_STRETCHED_TEXTURE:
            m_renderDrawer->renderStrechFullSprite(m_tex, m_rect, 255);
            break;
    }
}

void GuiButton::setTextAlignHorizontal(GuiTextAlignHorizontal value)
{
    m_textAlignHorizontal = value;
}

void GuiButton::setRenderKind(GuiRenderKind value)
{
    m_renderKind = value;
}

void GuiButton::setTextDrawer(cTextDrawer *cTextDrawer)
{
    m_textDrawer = cTextDrawer;
}

void GuiButton::setTexture(Texture *tex)
{
    m_tex = tex;
}

void GuiButton::drawText() const
{
    Color textColor = m_focus ? m_theme.textColorHover : m_theme.textColor;
    if (!m_enabled) {
        textColor = m_theme.disabledTextColor;
    }

    switch (m_textAlignHorizontal) {
        case GuiTextAlignHorizontal::CENTER:
            if (m_pressed) {
                m_textDrawer->drawTextCenteredInBox(m_buttonText.c_str(), m_rect, textColor, 1, 1);
            }
            else {
                m_textDrawer->drawTextCenteredInBox(m_buttonText.c_str(), m_rect, textColor);
            }
            break;
        case GuiTextAlignHorizontal::LEFT:
            if (m_pressed) {
                m_textDrawer->drawText(m_rect.getX() + 1, m_rect.getY() + 1, textColor, m_buttonText.c_str());
            }
            else {
                m_textDrawer->drawText(m_rect.getX(), m_rect.getY(), textColor, m_buttonText.c_str());
            }
            break;
    }
}

void GuiButton::nextRenderKind()
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

void GuiButton::toggleTextAlignHorizontal()
{
    if (m_textAlignHorizontal == GuiTextAlignHorizontal::CENTER) {
        m_textAlignHorizontal = GuiTextAlignHorizontal::LEFT;
    }
    else {
        m_textAlignHorizontal = GuiTextAlignHorizontal::CENTER;
    }
}

void GuiButton::onNotifyMouseEvent(const s_MouseEvent &event)
{
    switch (event.eventType) {
        case MOUSE_MOVED_TO:
            onMouseMovedTo(event);
            break;
        case MOUSE_RIGHT_BUTTON_CLICKED:
            onMouseRightButtonClicked(event);
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

void GuiButton::onMouseMovedTo(const s_MouseEvent &event)
{
    m_focus = m_rect.isPointWithin(event.coords);
}

void GuiButton::onMouseRightButtonClicked(const s_MouseEvent &)
{
    if (game.m_gameSettings->isDebugMode()) {

        // TODO: replace with code in onNotifyKeyboardEvent
        // DEBUG: Shift through render kinds

        // if (key[SDL_SCANCODE_LSHIFT]) {
        //     if (m_focus) toggleTextAlignHorizontal();
        // } else {
        if (m_focus) {
            nextRenderKind();
        }
        // }
    }
    if (m_focus) {
        if (m_enabled && m_onRightMouseButtonClickedAction) {
            m_onRightMouseButtonClickedAction();
        }
    }
}

void GuiButton::onMouseRightButtonPressed(const s_MouseEvent &)
{
    if (m_enabled) {
        m_pressed = m_focus;
    }
}

void GuiButton::onMouseLeftButtonPressed(const s_MouseEvent &)
{
    if (m_enabled) {
        m_pressed = m_focus;
    }
}

void GuiButton::onMouseLeftButtonClicked(const s_MouseEvent &) {
    if (m_focus) {
        if (m_enabled && m_onLeftMouseButtonClickedAction) {
            m_onLeftMouseButtonClickedAction();
            // un-press the button
            m_pressed = false;
        }
    }
}

void GuiButton::setOnLeftMouseButtonClickedAction(std::function<void()> action)
{
    m_onLeftMouseButtonClickedAction = std::move(action);
}

void GuiButton::setOnRightMouseButtonClickedAction(std::function<void()> action)
{
    m_onRightMouseButtonClickedAction = std::move(action);
}

void GuiButton::setEnabled(bool value)
{
    m_enabled = value;
}

void GuiButton::onNotifyKeyboardEvent(const cKeyboardEvent &)
{
}
