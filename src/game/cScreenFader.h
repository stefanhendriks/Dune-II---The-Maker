#pragma once

#include <SDL2/SDL.h>   //uint8

class Color;

enum class eFadeAction { None, FadeIn, FadeOut };

class cScreenFader {
public:
    cScreenFader();
    void startFadeIn();
    void startFadeOut();
    void startFadeNone();
    void update();
    bool isFading() const;
    Uint8 getAlpha() const;
    eFadeAction getAction() const;
    void inititialize();

    Color getColorFadeSelected(int r, int g, int b, bool rFlag = true, bool gFlag = true, bool bFlag = true);

private:
    eFadeAction m_action;
    Uint8 m_alpha;
    float m_fadeSelect;                 // fade color when selected
    bool m_fadeSelectDir;               // fade select direction
};