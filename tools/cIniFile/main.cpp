#include "cIniFile.h"
#include <vector>
#include <iostream>

using namespace std;
  
int main()  
{  
    cIniFile cini("./test.ini");  
  
    std::string str1 = cini.getStr("system","param1");
    std::string str2 = cini.getStr("system","param2");
    std::string str3 = cini.getStr("system","param3");
    
    std::string str4 = cini.getStr("config","param3");
    int str5 = cini.getInt("config","param4");

    std::cout << "param1 " << str1 << std::endl;
    std::cout << "param2 " << str2 << std::endl;
    std::cout << "param3 " << str3 << std::endl;

    std::cout << "param3 " << str4 << std::endl;
    std::cout << "param4 as int " << str5 << std::endl;

    return 0;  
}  
