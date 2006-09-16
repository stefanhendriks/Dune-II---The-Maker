/*
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

