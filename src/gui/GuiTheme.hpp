#pragma once

#include "drawers/SDLDrawer.hpp"

struct GuiTheme {
    Color fillColor;
    Color borderLight;
    Color borderDark;
    Color textColor;
    Color textColorHover;
    Color textColorShadow;

    static GuiTheme Light() {
        return {
            Color{176, 176, 196,255},
            Color{252, 252, 252,255},
            Color{84, 84, 120,255},
            Color::white(),
            Color{255, 0, 0,255},
            Color::black()
        };
    }
    static GuiTheme Dark() {
        return {
            Color{84, 84, 120,255},
            Color{40, 40, 40,255},
            Color{176, 176, 196,255},
            Color::white(),
            Color{255, 0, 0,255},
            Color::black()
        };
    }
    static GuiTheme Inactive() {
        return {
            Color{84, 84, 120,255},
            Color{40, 40, 40,255},
            Color{176, 176, 196,255},
            Color{192, 192, 192,255},
            Color{128, 128, 128,255},
            Color::black()
        };
    }
};
