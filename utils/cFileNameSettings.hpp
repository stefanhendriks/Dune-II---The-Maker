#pragma once

#include <string>
#include <map>

enum class EFILENAME {ARRAKEEN, BENEGESS, SMALL, GFXDATA, GFXINTER, GFXWORLD, GFXMENTAT, GFXAUDIO};

class cFileNameSettings
{
public:
    cFileNameSettings(const std::string &_path);
    void addRessources(std::map<EFILENAME, std::string> _transfertMap);
    ~cFileNameSettings();
    bool fileExists();
    const std::string getName(EFILENAME fileName);
    const std::string getFullName(EFILENAME fileName);

private:
    std::string path;
    std::map<EFILENAME, std::string> m_EnumToString;
};