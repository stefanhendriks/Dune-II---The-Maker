/**
 * @file GuiConsole.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include <memory>
#include <string>

class SDLDrawer;
class cTextDrawer;
class cNotificationArea;
class GuiWindow;
class GuiTextInput;
class GuiConsoleMessageParser;
class cKeyboardEvent;
struct s_MouseEvent;

class GuiConsole {
public:
    GuiConsole(SDLDrawer* sdlDrawer,
               cTextDrawer* textDrawer,
               cNotificationArea* notificationArea,
               int screenWidth,
               int screenHeight);
    ~GuiConsole();

    void toggle();
    bool isVisible() const;
    bool isKeyboardCaptured() const;

    void draw() const;
    void onNotifyKeyboardEvent(const cKeyboardEvent& event);
    void onNotifyMouseEvent(const s_MouseEvent& event);

private:
    void submit(const std::string& text);

    std::unique_ptr<GuiWindow> m_window;
    GuiTextInput* m_input = nullptr;
    std::unique_ptr<GuiConsoleMessageParser> m_messageParser;
    bool m_visible = false;
};
