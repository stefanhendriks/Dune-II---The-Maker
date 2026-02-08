#pragma once

#include "drawers/SDLDrawer.hpp"

struct GuiTheme {
    Color fillColor;
    Color borderLight;
    Color borderDark;
    Color background;
    Color textColor;
    Color textDarkColor;
    Color textColorHover;
    Color textColorShadow;
    Color disabled;

    static GuiTheme Light() {
        return {
            Color{176, 176, 196,255},   // fillColor
            Color{252, 252, 252,255},   // borderLight
            Color{84, 84, 120,255},     // borderDark 
            Color{32, 32, 32,255},      // background
            Color::white(),             // textColor
            Color{225, 177, 21,255},    // textDarkColor
            Color{255, 0, 0,255},       // textColorHover
            Color::black(),             // textColorShadow
            Color{128, 128, 128,255}    // disabled
        };
    }

    static GuiTheme Dark() {
        return {
            Color{84, 84, 120,255},
            Color{40, 40, 40,255},
            Color{176, 176, 196,255},
            Color{32, 32, 32,255},
            Color::white(),
            Color{225, 177, 21,255},
            Color{255, 0, 0,255},
            Color::black(),
            Color{128, 128, 128,255}
        };
    }

    static GuiTheme Inactive() {
        return {
            Color{84, 84, 120,255},
            Color{40, 40, 40,255},
            Color{176, 176, 196,255},
            Color{32, 32, 32,255},
            Color{192, 192, 192,255},
            Color{128, 128, 128,255},
            Color::black(),
            Color{128, 128, 128,255},
            Color{128, 128, 128,255}
        };
    }
};
