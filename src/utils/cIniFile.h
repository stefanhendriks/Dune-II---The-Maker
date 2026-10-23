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

// Orders strings ignoring ASCII case, so "HitPoints" and "hitpoints" are the same key
struct sCaseInsensitiveLess {
    bool operator()(const std::string &a, const std::string &b) const;
};

class cSection {
public:
    cSection();

    explicit cSection(const std::string &secName);

    ~cSection();

    bool addValue(const std::string &key, const std::string &value);

    // Sets (or replaces) a value without reporting a duplicate key
    void setValue(const std::string &key, const std::string &value);

    [[nodiscard]] bool hasValue(const std::string &key) const;

    // Removes a key, returns false when it did not exist
    bool removeValue(const std::string &key);

    // Keys in the order they were first added (file order)
    [[nodiscard]] const std::vector<std::string> &getKeys() const {
        return m_keyOrder;
    }

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
    std::map<std::string, std::string, sCaseInsensitiveLess> m_sectionConf;
    std::vector<std::string> m_keyOrder;
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

    // Removes a section, returns false when it did not exist
    bool removeSection(const std::string &section);

    // Removes a key from a section, returns false when the section or key did not exist
    bool removeValue(const std::string &section, const std::string &key);

    // Section names in the order they were added (file order)
    [[nodiscard]] const std::vector<std::string> &getSectionNames() const {
        return m_sectionOrder;
    }

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
    std::map<std::string, cSection, sCaseInsensitiveLess> m_mapConfig;
    std::vector<std::string> m_sectionOrder; // keeps sections in the order they were added, used by save()

    std::string m_actualSection, m_actualKey, m_actualValue;
};
