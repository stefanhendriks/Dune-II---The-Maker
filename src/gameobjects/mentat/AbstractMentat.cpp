/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#include "AbstractMentat.h"
#include "definitions.h"
#include "include/eGameState.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/RNG.hpp"
#include "context/GameContext.hpp"
#include "game/cGameInterface.h"
#include "game/cGameSettings.h"
#include "gui/GuiButton.h"
#include "utils/Graphics.hpp"
// #include "utils/common.h"
#include "utils/Log.h"
#include "drawers/cTextDrawer.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <format>
#include "include/cAssert.h"

// The mentat screen artwork was designed for 640x480. On higher resolutions it
// is centered, so the offset is half the extra pixels on each axis.
static constexpr int MENTAT_SCREEN_W = 640;
static constexpr int MENTAT_SCREEN_H = 480;

AbstractMentat::AbstractMentat(GameContext* ctx, bool canMissionSelect)
{
    d2tm_assert(ctx != nullptr);
    m_gameInterface = ctx->getGameInterface();
    gfxmentat = ctx->getGraphicsContext()->gfxmentat.get();
    m_textDrawer = ctx->getTextContext()->getBeneTextDrawer();
    m_renderDrawer = ctx->getSDLDrawer();
    iMentatSentence = -1;

    TIMER_Speaking = -1;
    TIMER_Mouth = 0;
    TIMER_Eyes = 0;
    TIMER_Other = 0;
    TIMER_movie = 0;

    iMentatMouth = 3;
    iMentatEyes = 3;
    iMovieFrame = -1;

    iBackgroundFrame = -1;

    house = GENERALHOUSE;

    gfxmovie = nullptr;
    leftButton = nullptr;
    rightButton = nullptr;
    // the mentat does not *own* the bitmaps
    leftButtonBmp = nullptr;
    rightButtonBmp = nullptr;
    leftGuiButton = nullptr;
    rightGuiButton = nullptr;

    if (canMissionSelect) {

        int length = m_textDrawer->getTextLength("Mission select");
        const cRectangle &toMissionSelectRect = *m_textDrawer->getAsRectangle(m_gameInterface->getGameSettings()->getScreenW() - length,
                                                m_gameInterface->getGameSettings()->getScreenH() - m_textDrawer->getFontHeight(),
                                                "Mission select");

        auto gui_btn_toMissionSelect = GuiButtonBuilder()
            .withRect(toMissionSelectRect)
            .withLabel("Mission select")
            .withTextDrawer(m_textDrawer)
            .withRenderer(m_renderDrawer)
            .withTheme(cGuiThemeBuilder().light().build())
            .withKind(GuiRenderKind::TRANSPARENT_WITHOUT_BORDER)
            .onClick([this] {
                m_gameInterface->setNextStateToTransitionTo(GAME_MISSIONSELECT);
            })
            .build();
        m_guiBtnToMissionSelect = std::move(gui_btn_toMissionSelect);
    }
    else {
        m_guiBtnToMissionSelect = nullptr;
    }

    state = INIT;

    offsetX = (m_gameInterface->getGameSettings()->getScreenW() - MENTAT_SCREEN_W) / 2;
    offsetY = (m_gameInterface->getGameSettings()->getScreenH() - MENTAT_SCREEN_H) / 2;
    movieTopleftX = offsetX + 256;
    movieTopleftY = offsetY + 120;
    memset(sentence, 0, sizeof(sentence));
    Logger::info(COMP_GAME, "AbstractMentat::AbstractMentat", "Initialized AbstractMentat with mission select button: {}", canMissionSelect);
}

AbstractMentat::~AbstractMentat()
{
    leftButtonBmp = nullptr;
    rightButtonBmp = nullptr;
    // m_guiBtnToMissionSelect will be a unique_ptr, auto-cleaned
    Logger::info(COMP_GAME, "AbstractMentat::~AbstractMentat", "Destroying AbstractMentat");
}

void AbstractMentat::think()
{
    if (state == INIT) {
        // no thinking for init state
        return;
    }

    if (state == SPEAKING) {
        if (TIMER_Speaking > 0) {
            TIMER_Speaking--;
        }
        else if (TIMER_Speaking == 0) {
            // calculate speaking stuff
            iMentatSentence += 2; // makes 0, 2, etc.

            // done talking
            if (iMentatSentence > 8) {
                state = AWAITING_RESPONSE;
                return;
            }

            // lentgh calculation of time
            int iLength = strlen(sentence[iMentatSentence]);
            iLength += strlen(sentence[iMentatSentence + 1]);

            // nothing to say, await response
            if (iLength < 2) {
                state = AWAITING_RESPONSE;
                return;
            }

            TIMER_Speaking = iLength * 12;
        }

        thinkMouth();
    }
    else {
        iMentatMouth = 0; // keep lips sealed
    }

    // eyes blink always
    thinkEyes();

    // regardless, think about movie animation
    thinkMovie();
}

void AbstractMentat::thinkMovie()
{
    if (gfxmovie != nullptr) {
        TIMER_movie++;

        if (TIMER_movie > 20) {
            iMovieFrame++;

            if (iMovieFrame == gfxmovie->getNumberOfFiles()) {
                iMovieFrame = 0;
            }
            TIMER_movie = 0;
        }
    }
}

void AbstractMentat::thinkEyes()
{
    if (TIMER_Eyes > 0) {
        TIMER_Eyes--;
    }
    else {
        int i = RNG::rnd(100);

        int iWas = iMentatEyes;

        if (i < 30) {
            iMentatEyes = 3;
        }
        else if (i < 60) {
            iMentatEyes = 0;
        }
        else {
            iMentatEyes = 4;
        }

        // its the same
        if (iMentatEyes == iWas) {
            iMentatEyes = RNG::rnd(4);
        }

        if (iMentatEyes != 4) {
            TIMER_Eyes = 90 + RNG::rnd(160);
        }
        else {
            TIMER_Eyes = 30;
        }
    }
}

void AbstractMentat::thinkMouth()  // MOUTH
{
    if (TIMER_Mouth > 0) {
        TIMER_Mouth--;
    }
    else {
        // still speaking
        if (TIMER_Speaking > 0) {

            int iOld = iMentatMouth;

            if (iMentatMouth == 0) { // mouth is shut
                // when mouth is shut, we wait a bit.
                if (RNG::rnd(100) < 45) {
                    iMentatMouth += (1 + RNG::rnd(4));
                }
                else {
                    TIMER_Mouth = 25; // wait
                }
            }
            else {
                iMentatMouth += (1 + RNG::rnd(4));
            }

            // correct any frame
            if (iMentatMouth > 4) {
                iMentatMouth -= 5;
            }

            // Test if we did not set the timer, when not, we changed stuff, and we
            // have to make sure we do not reshow the same animation.. which looks
            // odd!
            if (TIMER_Mouth <= 0) {
                // did not change mouth, force it
                if (iMentatMouth == iOld) {
                    iMentatMouth++;
                }

                // correct if needed:
                if (iMentatMouth > 4) {
                    iMentatMouth -= 5;
                }
            }
        }
        else {
            iMentatMouth = 0; // when there is no sentence, do not animate mouth
        }

        if (TIMER_Mouth <= 0) {
            TIMER_Mouth = 13 + RNG::rnd(15);
        }
    } // Animating mouth

}

void AbstractMentat::draw()
{
    m_renderDrawer->renderClearToColor(Color{7,7,15,255});
    m_renderDrawer->renderRectColor(offsetX-1, offsetY-1, 641, 481, Color{64, 64,89,255});
    m_renderDrawer->renderRectColor(offsetX-2, offsetY-2, 642, 482, Color{40,40,60,255});
    m_renderDrawer->renderRectColor(offsetX-3, offsetY-3, 643, 483, Color{0,0,0,255});

    // movie
    draw_movie();

    Texture *tmp = getBackgroundBitmap();
    cRectangle dest = {offsetX, offsetY, MENTAT_SCREEN_W, MENTAT_SCREEN_H};
    m_renderDrawer->renderStrechFullSprite(tmp, dest);

    draw_eyes();
    draw_mouth();
    draw_other();

    if (state == SPEAKING) {
        // draw text that is being spoken
        if (iMentatSentence >= 0) {
            m_textDrawer->drawText(offsetX + 17, offsetY + 17,Color{0, 0, 0,255},sentence[iMentatSentence]);
            m_textDrawer->drawText(offsetX + 17, offsetY + 17,Color{255, 255, 255,255},sentence[iMentatSentence]);

            m_textDrawer->drawText(offsetX + 17, offsetY + 37,Color{0, 0, 0,255},sentence[iMentatSentence+1]);
            m_textDrawer->drawText(offsetX + 17, offsetY + 36,Color{255, 255, 255,255},sentence[iMentatSentence+1]);
        }
    }

    if (state == AWAITING_RESPONSE) {
        if (leftGuiButton) {
            leftGuiButton->draw();
        }
        if (rightGuiButton){
            rightGuiButton->draw();
        }
    }

    if (m_guiBtnToMissionSelect) {
        m_guiBtnToMissionSelect->draw();
    }
}

Texture *AbstractMentat::getBackgroundBitmap() const
{
    if (iBackgroundFrame < 0) return nullptr;
    return gfxmentat->getTexture(iBackgroundFrame);
}

void AbstractMentat::draw_movie()
{
    if (gfxmovie == nullptr) return;
    if (iMovieFrame < 0) return;

    // drawing only, circulating is done in think function
    Texture *tmp = gfxmovie->getTexture(iMovieFrame);
    cRectangle dest = {movieTopleftX, movieTopleftY,tmp->w, tmp->h};
    m_renderDrawer->renderStrechFullSprite(tmp, dest);
}

void AbstractMentat::initSentences()
{
    memset(sentence, 0, sizeof(sentence));
}

void AbstractMentat::setSentence(int i, const char *text)
{
    snprintf(sentence[i], sizeof(sentence[i]), "%s", text);
    Logger::info(COMP_GAME, "AbstractMentat::setSentence", "Sentence[{}]={}", i, text);
}

void AbstractMentat::loadScene(const std::string &scene)
{
    gfxmovie = nullptr;

    char filename[255];
    snprintf(filename, sizeof(filename), "data/scenes/sdl_%s.dat", scene.c_str());

    gfxmovie = std::make_shared<Graphics>(m_renderDrawer->getRenderer(),filename);

    TIMER_movie = 0;
    iMovieFrame=0;

    if (gfxmovie != nullptr) {
        Logger::info(COMP_GAME, "AbstractMentat", "Successful loaded scene [{}]", filename);
        return;
    }

    gfxmovie=nullptr;
    Logger::error(COMP_GAME, "AbstractMentat", "Failed to load scene [{}]", filename);
}

void AbstractMentat::speak()
{
    TIMER_Speaking = 0;
    TIMER_Mouth = 0;
    iMentatSentence = -2; // ugh
    state = SPEAKING;
}

void AbstractMentat::buildLeftButton(Texture *bmp, int x, int y)
{
    delete leftButton;
    leftButton = new cRectangle(offsetX + x, offsetY + y, bmp->w, bmp->h);
    leftButtonBmp = bmp;
}

void AbstractMentat::buildRightButton(Texture *bmp, int x, int y)
{
    delete rightButton;
    rightButton = new cRectangle(offsetX + x, offsetY + y, bmp->w, bmp->h);
    rightButtonBmp = bmp;
}

void AbstractMentat::resetSpeak()
{
    speak();
}

void AbstractMentat::onNotifyMouseEvent(const s_MouseEvent &event)
{
    // Always keep button hover state current so focus is correct when buttons re-appear
    if (event.eventType == MOUSE_MOVED_TO) {
        if (m_guiBtnToMissionSelect) {
            m_guiBtnToMissionSelect->onNotifyMouseEvent(event);
        }
        if (leftGuiButton) {
            leftGuiButton->onNotifyMouseEvent(event);
        }
        if (rightGuiButton) {
            rightGuiButton->onNotifyMouseEvent(event);
        }
        return;
    }

    if (state == SPEAKING) {
        if (event.eventType == MOUSE_LEFT_BUTTON_CLICKED) {
            if (TIMER_Speaking > 0) {
                TIMER_Speaking = 1;
            }
        }
    }

    if (state == AWAITING_RESPONSE) {
        if (m_guiBtnToMissionSelect) {
            m_guiBtnToMissionSelect->onNotifyMouseEvent(event);
        }
        if (leftGuiButton) {
            leftGuiButton->onNotifyMouseEvent(event);
        }
        if (rightGuiButton) {
            rightGuiButton->onNotifyMouseEvent(event);
        }
    }
}

void AbstractMentat::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (m_guiBtnToMissionSelect) {
        m_guiBtnToMissionSelect->onNotifyKeyboardEvent(event);
    }
}