/**
 * @file GuiWindow.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include "GuiObject.h"
#include <string>
#include <vector>
#include <memory>

class SDLDrawer;
class cTextDrawer;

class GuiWindow : protected GuiObject {
public:
    explicit GuiWindow(SDLDrawer* drawer, const cRectangle &rect, cTextDrawer* _textDrawer);
    ~GuiWindow() noexcept override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    void draw() const override;

    bool hasFocusedInput() const;

    void addGuiObject(std::unique_ptr<GuiObject> guiObject);

    cRectangle getRelativeRect(int x, int y, int width, int height);

    void setTitle(const std::string &value) {
        title = value;
    }
    void setTheme(const GuiTheme& _theme) {
        GuiObject::setTheme(_theme);
    }
private:
    std::vector<std::unique_ptr<GuiObject>> gui_objects;
    std::string title;
    cTextDrawer* m_textDrawer = nullptr;
};

