/**
 * @file cFileValidator.cpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#include "cFileValidator.h"

#include "utils/Log.h"

#include <algorithm>
#include <iostream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

cFileValidator::cFileValidator(const std::string &path)
{
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

// A configured "<base>.dat" matches every "<base>*.dat" in the directory (so "sdl_data_5741.dat" is found for
// "sdl_data.dat"). The the exact name wins if present, otherwise the first match by name. Without any match the configured
// name is kept, so fileExists() reports it as missing.
static std::string resolveDatapackName(const std::string &path, const std::string &configured)
{
    const fs::path configuredPath(configured);
    if (configuredPath.extension() != ".dat") return configured;

    const std::string base = configuredPath.stem().string();
    std::vector<std::string> matches;
    std::error_code ec;
    for (const auto &entry : fs::directory_iterator(path, ec)) {
        if (!entry.is_regular_file(ec)) continue;
        const fs::path &file = entry.path();
        const std::string name = file.filename().string();
        if (file.extension() == ".dat" && name.starts_with(base)) {
            matches.push_back(name);
        }
    }
    if (matches.empty()) return configured;

    if (std::ranges::find(matches, configured) != matches.end()) {
        std::string others;
        for (const auto &name : matches) {
            if (name != configured) others += (others.empty() ? "" : ", ") + name;
        }
        if (!others.empty()) {
            Logger::info(COMP_INIT, "Load data", "Datapack '{}': using the exact name, other matching packs exist and are skipped: {}", configured, others);
        }
        return configured;
    }

    std::ranges::sort(matches);
    if (matches.size() > 1) {
        std::string others;
        for (size_t i = 1; i < matches.size(); i++) {
            others += (i > 1 ? ", " : "") + matches[i];
        }
        Logger::info(COMP_INIT, "Load data", "Datapack base name '{}': using '{}', other matching packs exist and are skipped: {}", base, matches[0], others);
    } else if (matches[0] != configured) {
        Logger::info(COMP_INIT, "Load data", "Datapack base name '{}': using '{}'", base, matches[0]);
    }
    return matches[0];
}

void cFileValidator::addResources(std::map<eGameDirFileName, std::string> _transfertMap)
{
    m_enumToString = _transfertMap;
    for (auto &[key, value] : m_enumToString) {
        value = resolveDatapackName(m_path, value);
    }
}

cFileValidator::~cFileValidator()
{
    m_enumToString.clear();
}

bool cFileValidator::fileExists()
{
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

const std::string cFileValidator::getName(eGameDirFileName fileName)
{
    return m_enumToString[fileName];
}

const std::string cFileValidator::getFullName(eGameDirFileName fileName)
{
    return m_path + "/" + m_enumToString[fileName];
}
