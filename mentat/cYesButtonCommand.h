#ifndef D2TM_CYESBUTTONCOMMAND_H
#define D2TM_CYESBUTTONCOMMAND_H

class cAbstractMentat;

#include "cButtonCommand.h"

class cYesButtonCommand : public cButtonCommand {

public:
    void execute(cAbstractMentat& mentat);

};


#endif //D2TM_CYESBUTTONCOMMAND_H
