#pragma once

#include "AbstractMentat.h"

class HarkonnenMentat : public AbstractMentat {
public:
    HarkonnenMentat(bool allowMissionSelect);
    void think();
private:
    void draw_mouth() override;
    void draw_eyes() override;
    void draw() override;
};
