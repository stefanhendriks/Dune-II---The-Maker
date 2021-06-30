#ifndef D2TM_CGAMESTATE_H
#define D2TM_CGAMESTATE_H

enum eGameStateType {
    GAMESTATE_SELECT_YOUR_NEXT_CONQUEST,
    GAMESTATE_SETUP_SKIRMISH_GAME,
};

class cGameState {

public:
    cGameState(cGame& theGame);
    virtual ~cGameState();

    virtual void think() = 0;
    virtual void draw() = 0;
    virtual void interact() = 0;

    virtual eGameStateType getType() = 0;

protected:
    // the state of the game itself
    cGame& game;

private:

};


#endif //D2TM_CGAMESTATE_H
