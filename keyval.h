/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2009 (c) code by Stefan Hendriks

  */
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


