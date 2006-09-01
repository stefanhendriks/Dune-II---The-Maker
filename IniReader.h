/*
    Ini class to read INI files
*/

class IniReader {

    
    private:
        KeyVal sectionMap;   
        char currentLine[MAX_LINE_LENGTH];
        
        int  currentSection;


        void ReadLine(FILE *file);
        int getSection();
        
    public:
        IniReader();

};