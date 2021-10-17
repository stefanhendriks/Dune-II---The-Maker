#ifndef D2TM_CGUIACTION_H
#define D2TM_CGUIACTION_H

class cGuiAction {
public:
    virtual ~cGuiAction() = default;
    virtual void execute() = 0;
};


#endif //D2TM_CGUIACTION_H
