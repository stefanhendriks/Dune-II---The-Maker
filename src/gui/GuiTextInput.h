#pragma once

#include "GuiObject.h"
#include <string>
#include <functional>

class cTextDrawer;

class GuiTextInput : public GuiObject {
public:
    GuiTextInput(const cRectangle& rect, cTextDrawer* textDrawer);

    void draw() const override;
    void onNotifyKeyboardEvent(const cKeyboardEvent& event) override;
    void onNotifyMouseEvent(const s_MouseEvent& event) override;

    const std::string& getText() const { return m_text; }
    void setText(const std::string& text) { m_text = text; }

    void setOnEnter(std::function<void(const std::string&)> callback) { m_onEnter = std::move(callback); }

private:
    std::string m_text;
    cTextDrawer *m_writer;
    bool m_focused = false;
    std::function<void(const std::string&)> m_onEnter;
};