#pragma once

#include "gui/cGuiAction.h"

class cGuiActionToGameState : public cGuiAction {
public:
    cGuiActionToGameState(int nextState, bool fadeOut);

    void execute() override;

private:
    bool fadeOut;
    int nextState;
};
