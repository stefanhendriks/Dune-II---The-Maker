#ifndef D2TM_CGUIACTIONTOGAMESTATE_H
#define D2TM_CGUIACTIONTOGAMESTATE_H

#include "../cGuiAction.h"

class cGuiActionToGameState : public cGuiAction {
public:
    cGuiActionToGameState(int nextState, bool fadeOut);

    void execute() override;

private:
    bool fadeOut;
    int nextState;
};


#endif //D2TM_CGUIACTIONTOGAMESTATE_H
