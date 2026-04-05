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

class cGuiThemeBuilder {

public:

    cGuiThemeBuilder &light() {
        m_fillColor = Color{176, 176, 196, 255};
        m_borderLight = Color{252, 252, 252, 255};
        m_borderDark = Color{84, 84, 120, 255};
        m_background = Color{32, 32, 32, 255};
        m_textColor = Color::white();
        m_textDarkColor = Color{225, 177, 21, 255};
        m_textColorHover = Color{255, 0, 0, 255};
        m_textColorShadow = Color::black();
        m_disabled = Color{128, 128, 128, 255};
        return *this;
    }

    cGuiThemeBuilder& withTextColor(Color textColor) {
        this->m_textColor = textColor;
        return *this;
    }

    GuiTheme build() const {
        return {
            m_fillColor,
            m_borderLight,
            m_borderDark,
            m_background,
            m_textColor,
            m_textDarkColor,
            m_textColorHover,
            m_textColorShadow,
            m_disabled
        };
    }

private:
    Color m_fillColor;
    Color m_borderLight;
    Color m_borderDark;
    Color m_background;
    Color m_textColor;
    Color m_textDarkColor;
    Color m_textColorHover;
    Color m_textColorShadow;
    Color m_disabled;
};