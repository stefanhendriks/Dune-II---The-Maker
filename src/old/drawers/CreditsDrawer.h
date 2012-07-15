/*
 * CreditsDrawer.h
 *
 *  Created on: Aug 16, 2009
 *      Author: Stefan
 */

#ifndef CREDITSDRAWER_H_
#define CREDITSDRAWER_H_

// the credits drawer takes state of a player, and draws the credits accordingly
#define SCROLLING_DIRECTION_UP 1
#define SCROLLING_DIRECTION_DOWN 2
#define SCROLLING_DIRECTION_NONE 3

class CreditsDrawer {
	public:
		CreditsDrawer(cPlayer *thePlayer);
		virtual ~CreditsDrawer();
		void destroy();

		void think();
    void draw();
    void setCredits(int amount);
    void setCreditsOfPlayer();
    BITMAP *getBitmap()
    {
        return bmp;
    }

private:
    cPlayer *player;
    BITMAP *bmp;
    void thinkAboutIndividualCreditOffsets();
    void drawCurrentCredits();
    void drawPreviousCredits();
    int getCreditDrawIdByChar(char c);
    int getXDrawingOffset(int amount);
    bool CreditsDrawer::isOffsetDirectionSet(int id);
    void initState();
    int CreditsDrawer::getNewCreditsToDraw();
    int CreditsDrawer::getSoundType();
    bool hasDrawnCurrentCredits();
    int getScrollingDirectionForOffset();
    void CreditsDrawer::slowdownRollSpeed();
    void CreditsDrawer::speedUpRollSpeed();

    int TIMER_money;
    float rollingSpeed;

    int newCreditsToDraw;
    int previousDrawnCredits;
    float previousCreditsRolledTo;

    float offset_credit[7];
    int offset_direction[7];

    bool initial;

    int soundType;
    float rollSpeed;

    int soundsMade;
};

#endif /* CREDITSDRAWER_H_ */
