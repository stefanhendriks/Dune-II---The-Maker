#pragma once

#include "AbstractMentat.h"

class AtreidesMentat : public AbstractMentat {
public:
    AtreidesMentat(bool allowMissionSelect);
    void think() override;

    protected:
    void draw_mouth() override;
    void draw_eyes() override;
    void draw_other() override;
    void draw() override;
    // void interact() override;
};
