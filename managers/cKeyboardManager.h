#ifndef CKEYBOARDMANAGER_H
#define	CKEYBOARDMANAGER_H

class cKeyboardManager {
public:
    cKeyboardManager();
    cKeyboardManager(const cKeyboardManager& orig);
    virtual ~cKeyboardManager();
    void interact();

private:
    void DEBUG_KEYS();
    void GAME_KEYS();
};

#endif	/* CKEYBOARDMANAGER_H */

