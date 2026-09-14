/**
 * @file GuiBar.h
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

#include "GuiObject.h"

#include <string>
#include <vector>

class SDLDrawer;

enum class GuiBarPlacement :char {
    HORIZONTAL = 0,
    VERTICAL = 1
};

class GuiBar : protected GuiObject {
public:
    explicit GuiBar(SDLDrawer* drawer, const cRectangle &rect, GuiBarPlacement placement, int iconSize = 40);
    ~GuiBar() noexcept override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    void draw() const override;

    void addGuiObject(std::unique_ptr<GuiObject> guiObject);
    void addAutoGuiObject(std::unique_ptr<GuiObject> guiObject);

    cRectangle getRelativeRect(int x, int y, int width, int height);

    void setTheme(const GuiTheme& _theme) {
        GuiObject::setTheme(_theme);
    }

    void beginPlacement(int value) {
        placementPosition = value;
    }
private:
    std::vector<std::unique_ptr<GuiObject>> gui_objects;
    GuiBarPlacement m_placement;
    int placementPosition;
    int heightBarSize;
    int halfMarginBetweenButtons;
};

