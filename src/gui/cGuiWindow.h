#pragma once

#include "cGuiObject.h"
#include "drawers/cTextDrawer.h"

#include <string>
#include <vector>

class cGuiWindow : cGuiObject {
public:
    explicit cGuiWindow(const cRectangle &rect);
    ~cGuiWindow();

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    void draw() const override;

    void addGuiObject(cGuiObject *guiObject);

    cRectangle getRelativeRect(int x, int y, int width, int height);

    void setTitle(const std::string &value) {
        title = value;
    }

private:
    std::vector<cGuiObject *> gui_objects;
    std::string title;
    cTextDrawer textDrawer;
};

