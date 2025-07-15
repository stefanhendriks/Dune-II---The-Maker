#pragma once

#include "cAbstractMentat.h"

class cAtreidesMentat : public cAbstractMentat {
protected:
    void draw_mouth() override;
    void draw_eyes() override;
    void draw_other() override;
    void draw() override;
    void interact() override;

public:
    cAtreidesMentat(bool allowMissionSelect);

    void think() override;
};
