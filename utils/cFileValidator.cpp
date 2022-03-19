#include "cFileValidator.h"

#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

cFileValidator::cFileValidator(const std::string &path) {
    m_path = path;
    // m_enumToString[eGameDirFileName::ARRAKEEN] = "arrakeen.fon";
    // m_enumToString[eGameDirFileName::BENEGESS] = "benegess.fon";
    // m_enumToString[eGameDirFileName::SMALL] = "small.ttf";
    // m_enumToString[eGameDirFileName::GFXDATA] = "gfxdata.dat";
    // m_enumToString[eGameDirFileName::GFXINTER] = "gfxinter.dat";
    // m_enumToString[eGameDirFileName::GFXWORLD] = "gfxworld.dat";
    // m_enumToString[eGameDirFileName::GFXMENTAT] = "gfxmentat.dat";
    // m_enumToString[eGameDirFileName::GFXAUDIO] = "gfxaudio.dat";
}

void cFileValidator::addRessources(std::map<eGameDirFileName, std::string> _transfertMap) {
    m_enumToString = _transfertMap;
}

cFileValidator::~cFileValidator() {
    m_enumToString.clear();
}

bool cFileValidator::fileExists() {
    // check if directory exists first
    fs::directory_entry entry_sandbox{m_path};
    if (!entry_sandbox.exists()) {
        std::cerr << "Error: '" << m_path << "' directory not found" << std::endl;
        return false;
    }

    // test if all key/values are properly set up
    bool result = true;
    for (const auto&[key, value]: m_enumToString) {
        if (value.empty()) {
            std::cerr << "Key: '" << eGameDirFileNameString(key) << "' has no filename configured." << std::endl;
            result = false;
        }
        std::string fullPath = m_path + "/" + value;
        if (!fs::exists(fullPath)) {
            std::cerr << "Key: '" << eGameDirFileNameString(key) << "' configured filename '" << value << "' which cannot be found. Path is '" << fullPath << "'." << std::endl;
            result = false;
        }
    }
    return result;
}

const std::string cFileValidator::getName(eGameDirFileName fileName) {
    return m_enumToString[fileName];
}

const std::string cFileValidator::getFullName(eGameDirFileName fileName) {
    return m_path + "/" + m_enumToString[fileName];
}
