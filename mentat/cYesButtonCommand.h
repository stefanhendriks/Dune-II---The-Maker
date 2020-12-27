#ifndef D2TM_CYESBUTTONCOMMAND_H
#define D2TM_CYESBUTTONCOMMAND_H

class cAbstractMentat;

#include "cButtonCommand.h"

/**
 * This accepts house selection
 */
class cYesButtonCommand : public cButtonCommand {

public:
    void execute(cAbstractMentat& mentat) override;

};


#endif //D2TM_CYESBUTTONCOMMAND_H
