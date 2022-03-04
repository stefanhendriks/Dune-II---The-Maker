#include "cFileNameSettings.hpp"

#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

cFileNameSettings::cFileNameSettings(const std::string &_path)
{
    path = _path;
}

void cFileNameSettings::addRessources(std::map<EFILENAME, std::string> _transfertMap)
{
    m_EnumToString = _transfertMap;
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
        if (value.empty()) {
            std::cerr << "file has no name." << std::endl;
            return false;
        }
        if (!fs::exists(path + "/" + value)) {
            std::cerr << "file " << value << " not found." << std::endl;
            return false;
        }
    }
    // all good
    return true;
}

const std::string cFileNameSettings::getName(EFILENAME fileName)
{
    return m_EnumToString[fileName];
}

const std::string cFileNameSettings::getFullName(EFILENAME fileName)
{
    return path + "/" + m_EnumToString[fileName];
}
