#ifndef D2TM_CGUIACTIONSTARTNEWSKIRMISHGAME_H
#define D2TM_CGUIACTIONSTARTNEWSKIRMISHGAME_H

#include "../../gamestates/cSetupSkirmishGameState.h"
#include "../cGuiAction.h"

class cGuiActionStartNewSkirmishGame : public cGuiAction {
public:
    cGuiActionStartNewSkirmishGame(cSetupSkirmishGameState * skirmishGameState);
    void execute() override;

private:
    cSetupSkirmishGameState * skirmishGameState;
};


#endif //D2TM_CGUIACTIONSTARTNEWSKIRMISHGAME_H
