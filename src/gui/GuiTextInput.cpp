#include "gui/GuiTextInput.h"
#include "drawers/cTextDrawer.h"
#include "include/cAssert.h"

#include <SDL.h>

#include <algorithm>

namespace {
    constexpr int kHorizontalPadding = 4;
    constexpr int kSecurityPadding = 5;
    constexpr Uint32 kCursorBlinkHalfPeriodMs = 500;

    bool shouldRenderCursor() {
        return ((SDL_GetTicks() / kCursorBlinkHalfPeriodMs) % 2u) == 0u;
    }

    void eraseLastUtf8Codepoint(std::string& text) {
        if (text.empty()) {
            return;
        }

        text.pop_back();
        while (!text.empty() && (static_cast<unsigned char>(text.back()) & 0xC0u) == 0x80u) {
            text.pop_back();
        }
    }
}

GuiTextInput::GuiTextInput(SDLDrawer* drawer,const cRectangle& rect, cTextDrawer* textDrawer)
    : GuiObject(drawer, rect), m_writer(textDrawer)
{
    d2tm_assert(drawer != nullptr);
    d2tm_assert(textDrawer != nullptr);
}

void GuiTextInput::draw() const
{
    m_renderDrawer->renderRectFillColor(m_rect, m_theme.background);
    if (m_focused) {
        cRectangle focusRect(m_rect.getX() - 3, m_rect.getY() - 3, m_rect.getWidth() + 6, m_rect.getHeight() + 6);
        m_renderDrawer->renderRectColor(focusRect, m_theme.textColorHover);
    }
    // Affiche un curseur si focus
    m_writer->drawText(m_rect.getX() + kHorizontalPadding, m_rect.getY() + 4, m_theme.textColor, getFittedDisplayText());
}

void GuiTextInput::onNotifyKeyboardEvent(const cKeyboardEvent& event)
{
    //std::cout << "GuiTextInput received keyboard event: " << event.toString() << std::endl;
    if (!m_focused) return;
    //std::cout << "GuiTextInput focused received keyboard event: " << event.toString() << std::endl;
    if (event.hasTextInput()) {
        const auto& textInput = event.getTextInput();
        if (canAppendText(textInput)) {
            m_text += textInput;
        }
    } else if (event.isBackspace()) {
        eraseLastUtf8Codepoint(m_text);
    } else if (event.isEnter()) {
        if (m_onEnter) m_onEnter(m_text);
    }
}

void GuiTextInput::setText(const std::string& text)
{
    m_text = text;

    if (!m_writer) {
        return;
    }

    while (!m_text.empty() && m_writer->getTextLength(m_text) > getMaxTextPixelWidth()) {
        m_text.pop_back();
    }
}

int GuiTextInput::getMaxTextPixelWidth() const
{
    return std::max(0, m_rect.getWidth() - (kHorizontalPadding * 2)-kSecurityPadding);
}

bool GuiTextInput::canAppendText(const std::string& text) const
{
    if (!m_writer || text.empty()) {
        return false;
    }

    std::string candidate = m_text;
    candidate += text;
    if (m_focused) {
        candidate.push_back('|');
    }

    return m_writer->getTextLength(candidate) <= getMaxTextPixelWidth();
}

std::string GuiTextInput::getFittedDisplayText() const
{
    std::string display = m_text;
    if (m_focused && shouldRenderCursor()) {
        display.push_back('|');
    }

    if (!m_writer) {
        return display;
    }

    while (!display.empty() && m_writer->getTextLength(display) > getMaxTextPixelWidth()) {
        display.erase(display.begin());
    }

    return display;
}

void GuiTextInput::onNotifyMouseEvent(const s_MouseEvent& event)
{
    //std::cout << "GuiTextInput received mouse event: " << s_MouseEvent::toString(event) << std::endl;
    if (event.eventType != eMouseEventType::MOUSE_LEFT_BUTTON_CLICKED) {
        return;
    }
    // Focus si clic dans la zone
    m_focused = m_rect.isPointWithin(event.coords);
}