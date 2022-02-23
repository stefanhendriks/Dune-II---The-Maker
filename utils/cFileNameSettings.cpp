#include "cFileNameSettings.hpp"

#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

cFileNameSettings::cFileNameSettings(const std::string &_path)
{
    path = _path;
    m_EnumToString[ENUMFILENAME::arrakeen] = "arrakeen.fon";
    m_EnumToString[ENUMFILENAME::benegess] = "benegess.fon";
    m_EnumToString[ENUMFILENAME::small] = "small.ttf";
    m_EnumToString[ENUMFILENAME::gfxdata] = "gfxdata.dat";
    m_EnumToString[ENUMFILENAME::gfxinter] = "gfxinter.dat";
    m_EnumToString[ENUMFILENAME::gfxworld] = "gfxworld.dat";
    m_EnumToString[ENUMFILENAME::gfxmentat] = "gfxmentat.dat";
    m_EnumToString[ENUMFILENAME::gfxaudio] = "gfxaudio.dat";
}

cFileNameSettings::~cFileNameSettings()
{
    m_EnumToString.clear();
}

bool cFileNameSettings::fileExists()
{
    //test data repertory
    fs::directory_entry entry_sandbox { path };
    if (!entry_sandbox.exists()) {
        std::cerr << path << "/ repertory not found" << std::endl;
        return false;
    }
    // test each files
    for (const auto& [key, value] : m_EnumToString) {
        if (!fs::exists(path + "/" + value)) {
            std::cerr << "file " << value << " not found." << std::endl;
            return false;
        }
    }
    // all good
    return true;
}

const std::string cFileNameSettings::getName(ENUMFILENAME fileName)
{
    return m_EnumToString[fileName];
}

const std::string cFileNameSettings::getFullName(ENUMFILENAME fileName)
{
    return path + "/" + m_EnumToString[fileName];
}
