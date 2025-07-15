#pragma once

class cAbstractMentat;

#include "cButtonCommand.h"

class cRepeatButtonCommand : public cButtonCommand {

public:
    void execute(cAbstractMentat &mentat) override;
};
