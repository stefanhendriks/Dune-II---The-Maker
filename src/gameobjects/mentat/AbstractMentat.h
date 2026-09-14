/**
 * @file AbstractMentat.h
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

#include "utils/cRectangle.h"
#include "observers/cInputObserver.h"
#include "utils/Graphics.hpp"

#include <string>
#include <memory>
#include <SDL3_ttf/SDL_ttf.h>

class GuiButton;
struct SDL_Surface;
class Texture;
class GameContext;
class cGameInterface;
class SDLDrawer;
class cTextDrawer;

enum eMentatState {
    INIT,               // for loading data (Default state)
    SPEAKING,           // the mentat is speaking about whatever
    AWAITING_RESPONSE,  // the mentat is done speaking, the user needs to click something (yes/no, proceed/repeat, etc)
    DESTROY,            // the mentat became 'invalid' (BeneGeserit-><house> transition)
};

class AbstractMentat : public cInputObserver {
public:
    AbstractMentat(GameContext* ctx, bool canMissionSelect);
    virtual ~AbstractMentat();

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    virtual void draw() = 0;
    virtual void think() = 0;

    void loadScene(const std::string &scene);

    Texture *getBackgroundBitmap() const;

    void initSentences();
    void speak();
    void setSentence(int i, const char text[256]);
    void thinkMouth();
    void thinkEyes();
    void thinkMovie();
    void setHouse(int value) {
        house = value;
    }
    int getHouse() {
        return house;
    }
    void resetSpeak();
protected:
    virtual void draw_mouth() = 0;

    virtual void draw_eyes() = 0;

    virtual void draw_other() = 0;

    void draw_movie();

    // Timed animation
    int TIMER_Mouth;
    int TIMER_Eyes;
    int TIMER_Other;
    int TIMER_Speaking;

    // Movie playback (scene's from datafile)
    int TIMER_movie;
    int iMovieFrame;

    int iBackgroundFrame;

    // draw 2 sentences at once, so 0 1, 2 3, 4 5, 6 7, 8 9
    char sentence[10][255];

    int iMentatSentence;  // = sentence to draw and speak with (-1 = not ready)

    int iMentatMouth;
    int iMentatEyes;

    std::shared_ptr<Graphics> gfxmovie;

    eMentatState state;
    cTextDrawer* m_textDrawer = nullptr;
    SDLDrawer* m_sdlDrawer;

    cRectangle *leftButton;
    cRectangle *rightButton;

    Texture *leftButtonBmp;
    Texture *rightButtonBmp;

    void buildLeftButton(Texture *bmp, int x, int y);
    void buildRightButton(Texture *bmp, int x, int y);
    std::unique_ptr<GuiButton> leftGuiButton;
    std::unique_ptr<GuiButton> rightGuiButton;
    int house;
    int offsetX;
    int offsetY;
    int movieTopleftX;
    int movieTopleftY;

    std::unique_ptr<GuiButton> m_guiBtnToMissionSelect;
    Graphics* gfxmentat;
    cGameInterface* m_gameInterface = nullptr;
};
