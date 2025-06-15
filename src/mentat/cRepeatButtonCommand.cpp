#include "cRepeatButtonCommand.h"

#include "cAbstractMentat.h"

void cRepeatButtonCommand::execute(cAbstractMentat &mentat) {
    mentat.resetSpeak();
}

