/**
 * @file cIniFile.h
 *
 * Dune 2 - The Maker
 *
 * original @author Mira.
 * This modified file is now for Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * 
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include <string>
#include <map>
#include <list>
#include <sstream>
#include <vector>

class cSection {
public:
    cSection();

    explicit cSection(const std::string &secName);

    ~cSection();

    bool addValue(const std::string &key, const std::string &value);

    [[nodiscard]] bool hasValue(const std::string &key) const;

    bool addData(const std::string &data);

    [[nodiscard]] std::string getStringValue(const std::string &key) const;

    [[nodiscard]] int getInt(const std::string &key) const;

    [[nodiscard]] double getDouble(const std::string &key) const;

    [[nodiscard]] bool getBoolean(const std::string &key) const;

    [[nodiscard]] std::vector<std::string> getData() const {
        return m_dataConfs;
    }

    template<typename T>
    T FromString(std::string value) const;

    // Writes "[name]", all key=value pairs and raw data lines
    void write(std::ostream &out) const;

private:
    std::string m_sectionName;
    std::vector<std::string> m_dataConfs;
    std::map<std::string, std::string> m_sectionConf;
};

class cIniFile {
public:
    cIniFile() = default;

    explicit cIniFile(const std::string &configFileName);

    ~cIniFile();

    bool load(const std::string &configFileName);

    // Saves all sections to the given UTF-8 file name
    bool save(const std::string &configFileName) const;

    std::string getStringValue(const std::string &section, const std::string &key) const;

    cSection getSection(const std::string &section) const;

    bool hasSection(const std::string &section) const;

    // Sets (or replaces) a key=value, creating the section if needed
    void setValue(const std::string &section, const std::string &key, const std::string &value);

    bool isLoadSuccess() const {
        return m_loadSuccess;
    }

private:
    bool isSectionName(std::string inputLine);
    bool isKeyValue(std::string inputLine);

    bool m_loadSuccess = false;

    std::string m_fileName;
    std::map<std::string, cSection> m_mapConfig;
    std::vector<std::string> m_sectionOrder; // keeps sections in the order they were added, used by save()

    std::string m_actualSection, m_actualKey, m_actualValue;
};
