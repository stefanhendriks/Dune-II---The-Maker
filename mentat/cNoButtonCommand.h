#ifndef D2TM_CNOBUTTONCOMMAND_H
#define D2TM_CNOBUTTONCOMMAND_H

class cAbstractMentat;

#include "cButtonCommand.h"

class cNoButtonCommand : public cButtonCommand {

public:
    void execute(cAbstractMentat& mentat);
};


#endif //D2TM_CNOBUTTONCOMMAND_H
