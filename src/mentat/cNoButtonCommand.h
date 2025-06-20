#pragma once

class cAbstractMentat;

#include "cButtonCommand.h"

/**
 * This rejects house selection
 */
class cNoButtonCommand : public cButtonCommand {

public:
    void execute(cAbstractMentat &mentat) override;
};
