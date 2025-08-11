#include "GuiButton.h"

#include "d2tmc.h"
#include "drawers/SDLDrawer.hpp"

GuiButton::GuiButton(const cRectangle &rect, const std::string &btnText)
    : GuiObject(rect)
    , m_textDrawer(nullptr)
    , m_buttonText(btnText)
    , m_renderKind(GuiRenderKind::OPAQUE_WITHOUT_BORDER)
    , m_textAlignHorizontal(GuiTextAlignHorizontal::CENTER)
    , m_onLeftMouseButtonClickedAction(nullptr)
    , m_onRightMouseButtonClickedAction(nullptr)
    , m_tex(nullptr)
    , m_focus(false)
    // , m_guiColorButton(Color{176, 176, 196,255})
    // , m_guiColorBorderLight(Color{252, 252, 252,255})
    // , m_guiColorBorderDark(Color{84, 84, 120,255})
    // , m_textColor(Color{255, 255, 255,255}) // default white color
    // , m_textColorHover(Color{255, 0, 0,255})
    , m_pressed(false)
    , m_enabled(true)   // by default always enabled
{
}

/*
GuiButton::GuiButton(const cTextDrawer &textDrawer, const cRectangle &rect, const std::string &btnText,
                       eGuiButtonRenderKind renderKind)
    : GuiButton(textDrawer, rect, btnText)
{
    m_renderKind = renderKind;
}
*/

// cGuiButton::cGuiButton(const cTextDrawer &textDrawer, const cRectangle &rect, const std::string &btnText, Color gui_colorButton,
//                        Color gui_colorBorderLight, Color gui_colorBorderDark)
//     : cGuiButton(textDrawer, rect, btnText)
// {
//     m_guiColorButton = gui_colorButton;
//     m_guiColorBorderLight = gui_colorBorderLight;
//     m_guiColorBorderDark = gui_colorBorderDark;
// }

GuiButton::~GuiButton()
{
    // delete m_onLeftMouseButtonClickedAction;
}

void GuiButton::draw() const
{
    switch (m_renderKind) {
        case OPAQUE_WITHOUT_BORDER:
            renderDrawer->renderRectFillColor(m_rect.getX(), m_rect.getY(), m_rect.getWidth(), m_rect.getHeight(), m_theme.fillColor);
            drawText();
            break;
        case TRANSPARENT_WITHOUT_BORDER:
            drawText();
            break;
        case OPAQUE_WITH_BORDER:
            renderDrawer->renderRectFillColor(m_rect.getX(), m_rect.getY(), m_rect.getWidth(), m_rect.getHeight(), m_theme.fillColor);
            if (m_pressed) {
                renderDrawer->gui_DrawRectBorder(m_rect, m_theme.borderDark, m_theme.borderLight);
            }
            else {
                renderDrawer->gui_DrawRectBorder(m_rect, m_theme.borderLight, m_theme.borderDark);
            }
            drawText();
            break;
        case TRANSPARENT_WITH_BORDER:
            if (m_pressed) {
                renderDrawer->gui_DrawRectBorder(m_rect, m_theme.borderDark, m_theme.borderLight);
            }
            else {
                renderDrawer->gui_DrawRectBorder(m_rect, m_theme.borderLight, m_theme.borderDark);
            }
            drawText();
            break;
        case WITH_TEXTURE:
            renderDrawer->renderSprite(m_tex, m_rect.getX(),m_rect.getY());
            break;
    }
}

// bool GuiButton::hasFocus()
// {
//     return m_focus;
// }

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
        textColor = m_focus ? m_theme.borderDark : m_theme.borderLight;
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

// void cGuiButton::setGui_ColorButton(Color value)
// {
//     m_guiColorButton = value;
// }

// void cGuiButton::setTextColor(Color value)
// {
//     m_textColor = value;
// }

// void cGuiButton::setTextColorHover(Color value)
// {
//     m_textColorHover = value;
// }

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
    if (game.isDebugMode()) {
        // LOST in SDL2 port: no keyboard acces i this area of code @Mira
        // if (key[SDL_SCANCODE_LSHIFT]) { // TODO: replace with code in onNotifyKeyboardEvent
        //     if (m_focus) toggleTextAlignHorizontal();
        // } else {
        if (m_focus) nextRenderKind();
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

void GuiButton::onMouseLeftButtonClicked(const s_MouseEvent &)
{
    if (m_focus) {
        if (m_enabled && m_onLeftMouseButtonClickedAction) {
            m_onLeftMouseButtonClickedAction();
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

// void GuiButton::setOnLeftMouseButtonClickedAction(cGuiAction *action)
// {
//     m_onLeftMouseButtonClickedAction = [action]() {
//         if (action) action->execute();
//     };
// }

void GuiButton::setEnabled(bool value)
{
    m_enabled = value;
}

void GuiButton::onNotifyKeyboardEvent(const cKeyboardEvent &)
{

}

void GuiButton::setOnEnterAction(std::function<void()> action)
{
    m_onEnterAction = std::move(action);
}

void GuiButton::OnEnter()
{
    if (m_onEnterAction) {
        m_onEnterAction();
    }
}