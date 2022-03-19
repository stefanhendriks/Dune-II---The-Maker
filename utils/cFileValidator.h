#pragma once

#include <string>
#include <map>
#include <cassert>
#include "enums.h"

enum class eGameDirFileName {
    ARRAKEEN,
    BENEGESS,
    SMALL,
    GFXDATA,
    GFXINTER,
    GFXWORLD,
    GFXMENTAT,
    GFXAUDIO
};

static const char* eGameDirFileNameString(const eGameDirFileName &value) {
    switch (value) {
        case eGameDirFileName::ARRAKEEN: return "ARRAKEEN";
        case eGameDirFileName::BENEGESS: return "BENEGESS";
        case eGameDirFileName::SMALL: return "SMALL";
        case eGameDirFileName::GFXDATA: return "GFXDATA";
        case eGameDirFileName::GFXINTER: return "GFXINTER";
        case eGameDirFileName::GFXWORLD: return "GFXWORLD";
        case eGameDirFileName::GFXMENTAT: return "GFXMENTAT";
        case eGameDirFileName::GFXAUDIO: return "GFXAUDIO";
        default:
            assert(false);
            break;
    }
    return "";
}

class cFileValidator {
public:
    cFileValidator(const std::string &path);

    void addRessources(std::map<eGameDirFileName, std::string> _transfertMap);

    ~cFileValidator();

    bool fileExists();

    const std::string getName(eGameDirFileName fileName);

    const std::string getFullName(eGameDirFileName fileName);

private:
    std::string m_path;
    std::map<eGameDirFileName, std::string> m_enumToString;
};