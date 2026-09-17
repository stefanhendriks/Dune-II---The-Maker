/**
 * @file cScreenFader.h
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

#include <SDL3/SDL.h>   //uint8

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

    Color getColorFadeSelectedLimited(Color color, float minFade);
    Color getColorFadeSelected(int r, int g, int b, bool rFlag = true, bool gFlag = true, bool bFlag = true);

private:
    Color getColorFadeSelectedLimited(int r, int g, int b, float minFade, bool rFlag = true, bool gFlag = true, bool bFlag = true);
    eFadeAction m_action;
    Uint8 m_alpha;
    float m_fadeSelect;                 // fade color when selected
    bool m_fadeSelectDir;               // fade select direction
};