/*
 This class should take care of simple key/value things. 

 This could be simplified i think with STL/VECTOR i think, 
 but i do not have the time (yet) to figure that out.
*/

#include "d2tmh.h"


KeyVal::KeyVal() {
    memset(key, 0, sizeof(key));
    memset(value, -1, sizeof(value));
}

/*
 Set a key with value; change value when key already exists
 or create new key when key does not exists.
*/
void KeyVal::setValue(char ckey[KEY_LENGTH], int value) {
    
    int iKeySlot = getValue(ckey);

    if (iKeySlot <= -1) {
        iKeySlot = getNewSlot();
    }

    if (iKeySlot > -1 ) {
        key[0][iKeySlot] = value;
    }
}


/* 
 Return new slot id
*/
int KeyVal::getNewSlot() {
    for (int i = 0; i < MAX_KEYS; i++) {
        if (key[0][i] == '\0') {
            return i;
        }
    }
    return -1;
}

/*
 Return value of key
 */
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

