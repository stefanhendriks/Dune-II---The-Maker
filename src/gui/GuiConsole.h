#pragma once

#include <memory>
#include <string>

class SDLDrawer;
class cTextDrawer;
class cNotificationArea;
class GuiWindow;
class GuiTextInput;
class cKeyboardEvent;
struct s_MouseEvent;

class GuiConsole {
public:
    GuiConsole(SDLDrawer* renderDrawer,
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
    cNotificationArea* m_notificationArea = nullptr;
    bool m_visible = false;
};
