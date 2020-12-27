#ifndef D2TM_CREPEATBUTTONCOMMAND_H
#define D2TM_CREPEATBUTTONCOMMAND_H

class cAbstractMentat;

#include "cButtonCommand.h"

class cRepeatButtonCommand : public cButtonCommand {

public:
    void execute(cAbstractMentat& mentat);
};


#endif //D2TM_CREPEATBUTTONCOMMAND_H
