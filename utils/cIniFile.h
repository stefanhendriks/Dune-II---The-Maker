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

    std::string getStringValue(const std::string &key) const;

    std::list<std::string> getAllKeys() const;

    bool addValue(const std::string &key, const std::string &value);

    bool addData(const std::string &data);

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

    int getInt(const std::string &section, const std::string &key) const;

    double getDouble(const std::string &section, const std::string &key) const;

    bool getBoolean(const std::string &section, const std::string &key) const;

    std::list<std::string> getKeyFromSection(const std::string &section) const;

    std::list<std::string> getSectionsFromIni() const;

private:
    template<typename T>
    T FromString(const std::string &value) const;

    std::string getSectionName(std::string inputLine);

    bool isSectionName(std::string inputLine);

    bool isKeyValue(std::string inputLine);

    std::string m_fileName;
    std::map<std::string, cSection> m_mapConfig;
    std::string m_actualSection, m_actualKey, m_actualValue;
};
