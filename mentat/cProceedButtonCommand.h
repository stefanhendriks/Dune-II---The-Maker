#ifndef D2TM_CPROCEEDBUTTONCOMMAND_H
#define D2TM_CPROCEEDBUTTONCOMMAND_H

class cAbstractMentat;

#include "cButtonCommand.h"

/**
 * This is executed when a Mentat's 'proceed' button is pressed (from mission briefing)
 */
class cProceedButtonCommand : public cButtonCommand {

public:
    void execute(cAbstractMentat& mentat) override;

};


#endif //D2TM_CPROCEEDBUTTONCOMMAND_H
