#pragma once

#include "gui/cGuiAction.h"

class cGuiActionSelectMission : public cGuiAction {
public:
    explicit cGuiActionSelectMission(int missionNr);

    void execute() override;
private:
    int m_missionNr;
};
