/*
 * CreditsDrawer.h
 *
 *  Created on: Aug 16, 2009
 *      Author: Stefan
 */

#pragma once

class GameContext;
class Graphics;
class cPlayer;
struct SDL_Surface;
class SDLDrawer;

// the credits drawer takes state of a player, and draws the credits accordingly

class CreditsDrawer {
public:
    explicit CreditsDrawer(GameContext* ctx, cPlayer *player);
    virtual ~CreditsDrawer();

    void thinkFast(); // set animation counters and such, time based. Also make sound when needed.

    void draw();
    void setCredits(int amount);
    void setCredits();
    void setPlayer(cPlayer *thePlayer);

private:
    cPlayer *m_player;  //
    GameContext* m_ctx;
    Graphics* m_gfxinter;
    Graphics* m_gfxdata;
    SDLDrawer* m_renderDrawer = nullptr;

    void thinkAboutIndividualCreditOffsets();
    void drawCurrentCredits(int drawX, int drawY);
    void drawPreviousCredits(int drawX, int drawY);
    int getCreditDrawId(char c);
    int getXDrawingOffset(int amount);

    int m_TIMER_money;	// timer needed to do logic

    float m_rollingSpeed;	// the 'rolling speed' of the credits. Need to be sped up when just starting
    // to withdrawn money.

    int m_currentCredits;		// current credits we want to draw
    int m_previousCredits;	// previous credits we wanted to draw

    // the offset of the current 'credit' being drawn (the Y offset)
    float m_offset_credit[7];
    int m_offset_direction[7]; // direction (0 = not yet determined / finished, 1 = UP, 2 = DOWN , 3 = DO NOTHING

    bool hasDrawnCurrentCredits();	// returns true whenever the new state has been finalized.

    bool m_initial;
    int m_soundType;
    float m_rollSpeed;
    int m_rolled; // how many credits did get rolled over?
    int m_soundsMade; // sounds made per time frame

    // topleft position of credits-bar bmp
    int m_drawX, m_drawY;
};
