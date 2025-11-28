#pragma once

#include "controls/cKeyboardEvent.h"
#include "observers/cInputObserver.h"
#include "sMouseEvent.h"

class cGame;
class GameContext;

enum eGameStateType {
    GAMESTATE_CHOOSE_HOUSE,                 // #define GAME_SELECT_HOUSE  8		// pick a house
    GAMESTATE_CREDITS,                      // #define GAME_CREDITS       17    // credits
    GAMESTATE_MAIN_MENU,                    // #define GAME_MENU          1     // in a menu    
    GAMESTATE_MENTAT,                       // #define GAME_WINNING     11      // when winning, do some cool animation
                                            // #define GAME_LOSEBRIEF   13      // briefing when losing
                                            // #define GAME_BRIEFING     3      // doing some briefing
    GAMESTATE_OPTIONS,                      // #define GAME_OPTIONS	      5     // options menu
    GAMESTATE_SELECT_MISSION,               // #define GAME_MISSIONSELECT 18    // mission select
    GAMESTATE_SELECT_YOUR_NEXT_CONQUEST,    // #define GAME_REGION		  6	    // select a region / select your next conquest
    GAMESTATE_SETUP_SKIRMISH,               // #define GAME_SETUPSKIRMISH 16    // set up a skirmish game
    GAMESTATE_TELLHOUSE,                    // #define GAME_TELLHOUSE	10		// tell about the house
    GAMESTATE_WINLOSE,                      // #define GAME_LOSING      14      // when losing, do something cool
                                            // #define GAME_WINBRIEF    12      // mentat chatter when won the mission
};

// #define GAME_INITIALIZE  -1      // initialize game
// #define GAME_OVER         0      // game over
// #define GAME_PLAYING      2      // playing the actual game

// #define GAME_EDITING      4      // running the editor
// #define GAME_INTRO	     9

// #define GAME_SKIRMISH    15      // playing a skirmish mission!


class cGameState : cInputObserver {

public:
    explicit cGameState(cGame &theGame, GameContext* ctx);
    virtual ~cGameState() = default;

    virtual void thinkFast() = 0;
    virtual void draw() const = 0;

    virtual void onNotifyMouseEvent(const s_MouseEvent &event) override = 0;
    virtual void onNotifyKeyboardEvent(const cKeyboardEvent &event) override = 0;
    // Do note: not every game state is a scenario observer!

    virtual eGameStateType getType() = 0;

protected:
    // the state of the game itself
    cGame &m_game;
    GameContext* m_ctx=nullptr;

private:

};

