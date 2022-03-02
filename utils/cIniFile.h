#pragma once

#include <string>
#include <map>
#include <list>
#include <sstream>
#include <vector>


class cSection
{
public:
    cSection();
    cSection(const std::string& secName);
    ~cSection();
    bool IsEmpty() const { return m_sectionConf.empty(); }
    std::string getValue(const std::string& key) const;
    std::string getData() const;    
    std::list<std::string> getAllKey() const;
    bool addValue(const std::string& key, const std::string& value);
    bool addData(const std::string& data);
private:
    std::string m_sectionName;
	std::vector<std::string> m_dataConfs;
    std::map<std::string, std::string> m_sectionConf;
};

class cIniFile
{
public:
    cIniFile();
    explicit cIniFile(const std::string& configFileName);
    ~cIniFile();
    bool load(const std::string& configFileName);
    //bool save(const std::string& savepath);
    std::string getStr(const std::string& section, const std::string& key) const;
    int getInt(const std::string& section, const std::string& key) const;
    double getDouble(const std::string& section, const std::string& key) const;
    bool getBoolean(const std::string& section, const std::string& key) const;

    std::list<std::string> getKeyFromSection(const std::string& section ) const; 
    std::list<std::string> getSectionsFromIni() const;
private:
    template<typename T> T FromString(const std::string& value) const;
    std::string getSectionName(std::string inputLine);

    bool isSectionName(std::string inputLine);
    bool isKeyValue(std::string inputLine);
    std::string m_fileName;
    std::map<std::string, cSection> m_mapConfig;
    std::string m_actualSection, m_actualKey, m_actualValue;
};
