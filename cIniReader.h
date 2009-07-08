/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2009 (c) code by Stefan Hendriks

  */

/*
    TODO: Make singleton, (perhaps not, Singleton= sort of anti-pattern).

	Ini class to read INI files
*/

class cIniReader {

    
    private:
        KeyVal sectionMap;   
        char currentLine[MAX_LINE_LENGTH];
        
        int  currentSection;


        void ReadLine(FILE *file);
        int getSection();
        
    public:
        cIniReader();

};

