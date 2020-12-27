#ifndef D2TM_CNOBUTTONCOMMAND_H
#define D2TM_CNOBUTTONCOMMAND_H

class cAbstractMentat;

#include "cButtonCommand.h"

/**
 * This rejects house selection
 */
class cNoButtonCommand : public cButtonCommand {

public:
    void execute(cAbstractMentat& mentat) override;
};


#endif //D2TM_CNOBUTTONCOMMAND_H
