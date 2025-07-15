#pragma once

class cAbstractMentat;

#include "cButtonCommand.h"

/**
 * This accepts house selection
 */
class cYesButtonCommand : public cButtonCommand {

public:
    void execute(cAbstractMentat &mentat) override;

};
