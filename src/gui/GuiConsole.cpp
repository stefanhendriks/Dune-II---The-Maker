#include "gui/GuiConsole.h"

#include "game/cNotificationArea.h"
#include "drawers/cTextDrawer.h"
#include "gui/GuiConsole/GuiConsoleMessageParser.h"
#include "gui/GuiTextInput.h"
#include "gui/GuiWindow.h"

#include <algorithm>

GuiConsole::GuiConsole(SDLDrawer* renderDrawer,
                       cTextDrawer* textDrawer,
                       cNotificationArea* notificationArea,
                       int screenWidth,
                       int screenHeight)
{
    const int width = std::max(260, std::min(520, screenWidth - 16));
    const int inputHeight = textDrawer->getFontHeight() + 4;
    const int height = inputHeight + 8;
    const int x = 8;
    const int y = screenHeight - height - 10;

    m_window = std::make_unique<GuiWindow>(renderDrawer, cRectangle(x, y, width, height), textDrawer);
    m_messageParser = std::make_unique<GuiConsoleMessageParser>(notificationArea);

    auto input = GuiTextInputBuilder()
        .withRect(m_window->getRelativeRect(8, 6, width - 16, inputHeight))
        .withTextDrawer(textDrawer)
        .withRenderer(renderDrawer)
        .withTheme(cGuiThemeBuilder().light().build())
        .onEnter([this](const std::string& text) {
            submit(text);
        })
        .build();

    m_input = input.get();
    m_window->addGuiObject(std::move(input));
}

GuiConsole::~GuiConsole() = default;

void GuiConsole::toggle()
{
    m_visible = !m_visible;
    if (m_input) {
        m_input->setFocused(m_visible);
    }
}

bool GuiConsole::isVisible() const
{
    return m_visible;
}

bool GuiConsole::isKeyboardCaptured() const
{
    return m_visible && m_window && m_window->hasFocusedInput();
}

void GuiConsole::draw() const
{
    if (!m_visible || !m_window) {
        return;
    }

    m_window->draw();
}

void GuiConsole::onNotifyKeyboardEvent(const cKeyboardEvent& event)
{
    if (!m_visible || !m_window) {
        return;
    }

    m_window->onNotifyKeyboardEvent(event);
}

void GuiConsole::onNotifyMouseEvent(const s_MouseEvent& event)
{
    if (!m_visible || !m_window) {
        return;
    }

    m_window->onNotifyMouseEvent(event);
}

void GuiConsole::submit(const std::string& text)
{
    if (m_messageParser) {
        m_messageParser->submit(text);
    }

    if (m_input) {
        m_input->setText("");
    }
}
