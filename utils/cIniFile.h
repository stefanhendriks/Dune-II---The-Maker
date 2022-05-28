#pragma once

#include <string>
#include <map>
#include <list>
#include <sstream>
#include <vector>

static const std::string SECTION_HOUSES = "HOUSES";
static const std::string SECTION_SETTINGS = "SETTINGS";

class cSection {
public:
    cSection();

    explicit cSection(const std::string &secName);

    ~cSection();

    std::string getStringValue(const std::string &key) const;

    bool addValue(const std::string &key, const std::string &value);

    bool addData(const std::string &data);

    [[nodiscard]] int getInt(const std::string &key) const;

    [[nodiscard]] double getDouble(const std::string &key) const;

    [[nodiscard]] bool getBoolean(const std::string &key) const;

    template<typename T>
    T FromString(std::string value) const;

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

    std::string getStringValue(const std::string &section, const std::string &key) const;

    cSection getSection(const std::string &section) const;

    bool hasSection(const std::string &section) const;

private:

    bool isSectionName(std::string inputLine);

    bool isKeyValue(std::string inputLine);

    std::string m_fileName;
    std::map<std::string, cSection> m_mapConfig;

    std::string m_actualSection, m_actualKey, m_actualValue;
};
