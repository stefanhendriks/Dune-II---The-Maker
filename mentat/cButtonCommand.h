#pragma once

#include "cAbstractMentat.h"

class cButtonCommand {
    public:
        virtual ~cButtonCommand() = default;
        virtual void execute(cAbstractMentat& mentat) = 0;
};
