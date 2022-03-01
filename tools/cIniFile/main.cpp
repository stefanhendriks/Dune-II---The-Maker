#include "cIniFile.h"
#include <vector>
#include <iostream>

using namespace std;
  
int main()  
{  
    cIniFile cini("./game.ini");  
  
    std::string str1 = cini.getStr("UNIT: SANDWORM","Appetite");
    std::string str2 = cini.getStr("UNIT: RAIDER","AttackFrequency");
    std::string str3 = cini.getStr("STRUCTURE: 4SLAB","Cost");
    
    std::string str4 = cini.getStr("SETTINGS","ScreenWidth");
    int str5 = cini.getInt("SETTINGS","ScreenHeight");

    std::cout << "param1 " << str1 << std::endl;
    std::cout << "param2 " << str2 << std::endl;
    std::cout << "param3 " << str3 << std::endl;

    std::cout << "param3 " << str4 << std::endl;
    std::cout << "param4 as int " << str5 << std::endl;

    return 0;  
}  
