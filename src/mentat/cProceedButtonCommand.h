#pragma once

class cAbstractMentat;

#include "cButtonCommand.h"

/**
 * This is executed when a Mentat's 'proceed' button is pressed (from mission briefing)
 */
class cProceedButtonCommand : public cButtonCommand {

public:
    void execute(cAbstractMentat& mentat) override;

};
