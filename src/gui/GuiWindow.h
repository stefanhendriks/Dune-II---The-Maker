#pragma once

#include "GuiObject.h"
#include "drawers/cTextDrawer.h"

#include <string>
#include <vector>
#include <memory>

class GuiWindow : protected GuiObject {
public:
    explicit GuiWindow(const cRectangle &rect);
    ~GuiWindow();

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    void draw() const override;

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
    cTextDrawer textDrawer;
};

