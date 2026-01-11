#pragma once

#include "GuiObject.h"
#include "drawers/cTextDrawer.h"

#include <string>
#include <vector>

enum class GuiBarPlacement :char {
    HORIZONTAL = 0,
    VERTICAL = 1
};

class GuiBar : protected GuiObject {
public:
    explicit GuiBar(const cRectangle &rect, GuiBarPlacement placement, int iconSize = 40);
    ~GuiBar() noexcept override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    void draw() const override;

    void addGuiObject(GuiObject *guiObject);

    cRectangle getRelativeRect(int x, int y, int width, int height);

    void setTheme(const GuiTheme& _theme) {
        GuiObject::setTheme(_theme);
    }

    void beginPlacement(int value) {
        placementPosition = value;
    }
private:
    std::vector<GuiObject *> gui_objects;
    GuiBarPlacement m_placement;
    int placementPosition;
    int heightBarSize;
    int halfMarginBetweenButtons;
};

