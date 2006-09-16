#ifndef KEYVAL_H

const int KEY_LENGTH = 25;
const int MAX_KEYS = 255;

class KeyVal {
    
    private:
        char key[KEY_LENGTH][MAX_KEYS];
        int  value[MAX_KEYS];
        
        int getNewSlot();
        
    public:
        KeyVal();
        int  getValue(char key[KEY_LENGTH]);
        void setValue(char key[KEY_LENGTH], int value);
};

#endif


