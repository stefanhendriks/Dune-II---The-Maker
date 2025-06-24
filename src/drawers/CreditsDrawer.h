/*
 * CreditsDrawer.h
 *
 *  Created on: Aug 16, 2009
 *      Author: Stefan
 */

#pragma once

class cPlayer;
//struct SDL_Surface;
struct SDL_Surface;

// the credits drawer takes state of a player, and draws the credits accordingly

class CreditsDrawer {
public:
    explicit CreditsDrawer(cPlayer *thePlayer);
    virtual ~CreditsDrawer();

    void thinkFast(); // set animation counters and such, time based. Also make sound when needed.

    void draw();
    void setCredits(int amount);
    void setCredits();

    // SDL_Surface *getBitmap() {
    //     return bmp;
    // }

    void setPlayer(cPlayer *thePlayer);

private:
    cPlayer *player;  //
    SDL_Surface *bmp;	 // a bitmap being drawn on

    void thinkAboutIndividualCreditOffsets();
    void drawCurrentCredits();
    void drawPreviousCredits();
    int getCreditDrawId(char c);
    int getXDrawingOffset(int amount);

    int TIMER_money;	// timer needed to do logic

    float rollingSpeed;	// the 'rolling speed' of the credits. Need to be sped up when just starting
    // to withdrawn money.

    int currentCredits;		// current credits we want to draw
    int previousCredits;	// previous credits we wanted to draw

    // the offset of the current 'credit' being drawn (the Y offset)
    float offset_credit[7];
    int offset_direction[7]; // direction (0 = not yet determined / finished, 1 = UP, 2 = DOWN , 3 = DO NOTHING

    bool hasDrawnCurrentCredits();	// returns true whenever the new state has been finalized.

    bool initial;
    int soundType;
    float rollSpeed;
    int rolled; // how many credits did get rolled over?
    int soundsMade; // sounds made per time frame

    // topleft position of credits-bar bmp
    int drawX, drawY;
};
