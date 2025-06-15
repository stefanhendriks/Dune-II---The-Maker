#pragma once

#include "cAbstractMentat.h"

/**
 * This seems to be similar to cGuiAction?
 */
class cButtonCommand {
    public:
        virtual ~cButtonCommand() = default;
        virtual void execute(cAbstractMentat& mentat) = 0;
};
