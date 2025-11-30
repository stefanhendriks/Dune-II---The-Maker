#include "game/cScreenFader.h"
#include "utils/Color.hpp"
#include <algorithm>

cScreenFader::cScreenFader() : m_action(eFadeAction::None), m_alpha(0)
{}

void cScreenFader::startFadeIn()
{
    m_action = eFadeAction::FadeIn;
    m_alpha = 0;
}

void cScreenFader::startFadeOut()
{
    m_action = eFadeAction::FadeOut;
    m_alpha = 250;
}

void cScreenFader::startFadeNone()
{
    m_alpha = 255;
    m_action = eFadeAction::None;
}

void cScreenFader::inititialize()
{
    m_fadeSelect = 1.0f;
    m_fadeSelectDir = true;    // fade select direction
    m_alpha = 0;             // 255 = opaque , anything else
    m_action = eFadeAction::FadeIn; // 0 = NONE, 1 = fade out (go to 0), 2 = fade in (go to 255)
}

void cScreenFader::update()
{
    if (m_action == eFadeAction::FadeIn) {
        if (m_alpha < 255) 
            m_alpha += 2;
        else
            m_action = eFadeAction::None;
    } 
    else if (m_action == eFadeAction::FadeOut) {
        if (m_alpha > 0)
            m_alpha -= 2;
        else 
            m_action = eFadeAction::None;
    }

    std::clamp(m_alpha,Uint8(0),Uint8(255));
    // Fading
    // assert(m_fadeAlpha >= kMinAlpha);
    // assert(m_fadeAlpha <= kMaxAlpha);

    // Fading / pulsating of selected stuff
    static constexpr float fadeSelectIncrement = 1 / 256.0f;
    if (m_fadeSelectDir) {
        m_fadeSelect += fadeSelectIncrement;
        // when 255, then fade back
        if (m_fadeSelect > 0.99) {
            m_fadeSelect = 1.0f;
            m_fadeSelectDir = false;
        }
        return;
    }

    m_fadeSelect -= fadeSelectIncrement;
    // not too dark, 0.03125
    if (m_fadeSelect < 0.3125f) {
        m_fadeSelectDir = true;
    }
}

bool cScreenFader::isFading() const
{
    return m_action != eFadeAction::None;
}

Uint8 cScreenFader::getAlpha() const
{
    return m_alpha;
}

eFadeAction cScreenFader::getAction() const
{
    return m_action;
}

Color cScreenFader::getColorFadeSelected(int r, int g, int b, bool rFlag, bool gFlag, bool bFlag)
{
    unsigned char desiredRed = rFlag ? r * m_alpha : r;
    unsigned char desiredGreen = gFlag ? g * m_alpha : g;
    unsigned char desiredBlue = bFlag ? b * m_alpha : b;
    return Color{desiredRed, desiredGreen, desiredBlue,255};
}