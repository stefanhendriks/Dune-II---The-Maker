#ifndef D2TM_CGUIACTIONTOMAINMENUSTATE_H
#define D2TM_CGUIACTIONTOMAINMENUSTATE_H

#include "../cGuiAction.h"

class cGuiActionToMainMenuState : public cGuiAction {
public:
    cGuiActionToMainMenuState();

    void execute() override;
    void setFadeOut(bool value) { fadeOut = value; };

private:
    bool fadeOut;
};


#endif //D2TM_CGUIACTIONTOMAINMENUSTATE_H
