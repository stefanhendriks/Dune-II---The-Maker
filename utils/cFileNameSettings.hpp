#pragma once

#include <string>
#include <map>

enum class ENUMFILENAME {arrakeen, benegess, small, gfxdata, gfxinter, gfxworld, gfxmentat, gfxaudio};

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