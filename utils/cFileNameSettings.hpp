#pragma once

#include <string>
#include <map>

enum class ENUMFILENAME {ARRAKEEN, BENEGESS, SMALL, GFXDATA, GFXINTER, GFXWORLD, GFXMENTAT, GFXAUDIO};

class cFileNameSettings
{
public:
    cFileNameSettings(const std::string &_path);
    ~cFileNameSettings();
    bool fileExists();
    const std::string getName(ENUMFILENAME fileName);
    const std::string getFullName(ENUMFILENAME fileName);

private:
    std::string path;
    std::map<ENUMFILENAME, std::string> m_EnumToString;
};