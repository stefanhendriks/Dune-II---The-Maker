#pragma once

#include "gamestates/cSetupSkirmishGameState.h"
#include "gui/cGuiAction.h"

class cGuiActionStartNewSkirmishGame : public cGuiAction {
  public:
    explicit cGuiActionStartNewSkirmishGame(cSetupSkirmishGameState * skirmishGameState);
    void execute() override;

  private:
    cSetupSkirmishGameState * skirmishGameState;
};
