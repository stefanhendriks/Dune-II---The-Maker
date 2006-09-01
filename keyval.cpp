#include "d2tmh.h"
#include "d2tmc.h" 

KeyVal::KeyVal() {
    memset(key, 0, sizeof(key));
    memset(value, -1, sizeof(value));
}

void KeyVal::setValue(char ckey[KEY_LENGTH], int value) {
    
    int iKeySlot = getValue(ckey);

    if (iKeySlot <= -1) {
        iKeySlot = getNewSlot();
    }

    if (iKeySlot > -1 ) {
        key[0][iKeySlot] = value;
    }
}

int KeyVal::getNewSlot() {
    for (int i = 0; i < MAX_KEYS; i++) {
        if (key[0][i] == '\0') {
            return i;
        }
    }
    return -1;
}

int KeyVal::getValue(char ckey[KEY_LENGTH]) {
    for (int i = 0; i < MAX_KEYS; i++) {
        char k[KEY_LENGTH];
        sprintf(k, "%s", key[0][i]);
        if (strcmp(k, ckey) == 0) {
            return i;
        }  
    }

    return -1;
}

