/**
 * @file cFileValidator.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include <string>
#include <map>
#include "include/cAssert.h"
#include "enums.h"

enum class eGameDirFileName {
    ARRAKEEN,
    BENEGESS,
    SMALL,
    GFXDATA,
    GFXINTER,
    GFXWORLD,
    GFXMENTAT,
    GFXAUDIO,
    GFXEDITOR
};

inline constexpr std::string eGameDirFileNameString(const eGameDirFileName &value)
{
    switch (value) {
        case eGameDirFileName::ARRAKEEN:
            return "ARRAKEEN";
        case eGameDirFileName::BENEGESS:
            return "BENEGESS";
        case eGameDirFileName::SMALL:
            return "SMALL";
        case eGameDirFileName::GFXDATA:
            return "GFXDATA";
        case eGameDirFileName::GFXINTER:
            return "GFXINTER";
        case eGameDirFileName::GFXWORLD:
            return "GFXWORLD";
        case eGameDirFileName::GFXMENTAT:
            return "GFXMENTAT";
        case eGameDirFileName::GFXAUDIO:
            return "GFXAUDIO";
        case eGameDirFileName::GFXEDITOR:
            return "GFXEDITOR";
        default:
            d2tm_assert(false);
            break;
    }
    return "";
}

class cFileValidator {
public:
    cFileValidator(const std::string &path);

    void addResources(std::map<eGameDirFileName, std::string> _transfertMap);

    ~cFileValidator();

    bool fileExists();

    const std::string getName(eGameDirFileName fileName);

    const std::string getFullName(eGameDirFileName fileName);

private:
    std::string m_path;
    std::map<eGameDirFileName, std::string> m_enumToString;
};