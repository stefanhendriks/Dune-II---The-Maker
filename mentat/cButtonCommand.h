#ifndef D2TM_CBUTTONCOMMAND_H
#define D2TM_CBUTTONCOMMAND_H

class cAbstractMentat;

class cButtonCommand {
    public:
        virtual ~cButtonCommand() = default;
        virtual void execute(cAbstractMentat& mentat) = 0;
};


#endif //D2TM_CBUTTONCOMMAND_H
