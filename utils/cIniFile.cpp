#include "cIniFile.h"
#include <fstream>
#include <iostream>
#include <algorithm>


static void Trim(std::string& str)
{
    str.erase(str.find_last_not_of(" \t")+1);         //suffixing spaces
    str.erase(0, str.find_first_not_of(" \t"));       //prefixing spaces
}


//
// cSection class
//

cSection::cSection()
{}

cSection::~cSection()
{
    m_sectionConf.clear();
    m_dataConfs.clear();
}

cSection::cSection(const std::string& secName) : m_sectionName(secName)
{}

bool cSection::addValue(const std::string& key, const std::string& value)
{
    if (m_sectionConf.find(value) != m_sectionConf.end()) {
        std::cout << "Key " << key << " already exist on section " << m_sectionName << std::endl;
        return false;
    }
    m_sectionConf[key] = value;
    return true;
}

bool cSection::addData(const std::string& data)
{
    m_dataConfs.push_back(data);
    return true;
}



std::string cSection::getValue(const std::string& key) const
{
    if (m_sectionConf.find(key) != m_sectionConf.end()) {
        return m_sectionConf.at(key);
    } else {
        std::cout << "key " << key << " didn't exist on section " << m_sectionName << std::endl;
        return std::string();
    }
}

std::list<std::string> cSection::getAllKey() const
{
    std::list<std::string> mList;
    for (auto& x: m_sectionConf) {
        mList.push_back(x.first);
    }
    return mList;
}


//
// cIniFile class
//

cIniFile::cIniFile(const std::string &configFileName)
    :m_fileName(configFileName){
    load(m_fileName);
}

cIniFile::~cIniFile() { }


bool cIniFile::load(const std::string& config)
{
    m_fileName = config;
    std::ifstream in(m_fileName.c_str());
    if (!in) {
        std::cout << "unable to open file " << m_fileName << std::endl;
        return false;
    }
    std::string line, secName, lastSecName;
    while (!in.eof()){
        // get raw line 
        getline(in, line);
        Trim(line);
        // commented line
        if (line.empty() || line.find(';') == 0 || line.find('#') == 0)
            continue;

        //test if new section
        if (isSectionName(line) && !m_actualSection.empty()) {
            // test if already exist
            if (m_mapConfig.find(m_actualSection) != m_mapConfig.end()) {
                std::cout << "section " << m_actualSection << " already exist" << std::endl;
                continue;
            }
            m_mapConfig[m_actualSection] = cSection(m_actualSection);
            continue;    
        }
        // test if key=value
        if (isKeyValue(line) && !m_actualSection.empty()) {
            m_mapConfig[m_actualSection].addValue(m_actualKey, m_actualValue);
            continue;
        }

        //none from forward so string is a data from section
        if (!m_actualSection.empty()) {
            m_mapConfig[m_actualSection].addData(line);
            continue;
        }

        //all tests are wrong
        std::cout << "error " << line << " or no section found" << std::endl;
    }
    return true;
}

bool cIniFile::isSectionName(std::string inputLine)
{
    size_t sec_begin_pos = inputLine.find('[');
    if (sec_begin_pos == std::string::npos || sec_begin_pos != 0){
        return false;
    }
    size_t sec_end_pos = inputLine.find(']', sec_begin_pos);
    if (sec_end_pos == std::string::npos){
        return false;
    }

    m_actualSection = (inputLine.substr(sec_begin_pos + 1, sec_end_pos - sec_begin_pos - 1));
    Trim(inputLine);
    return true;
}

bool cIniFile::isKeyValue(std::string inputLine)
{
    size_t keyPos = inputLine.find('=');
    if (keyPos == std::string::npos || keyPos == 0 || keyPos == inputLine.length()-1 ){
        m_actualKey.clear();
        m_actualValue.clear();
        return false;
    }
    std::string key = inputLine.substr(0, keyPos);
    Trim(key);
    m_actualKey = key;

    std::string value = inputLine.substr(keyPos+1);
    Trim(value);
    m_actualValue = value;

    if (m_actualKey.empty() || m_actualValue.empty()) {
        m_actualKey.clear();
        m_actualValue.clear();
        return false;
    }
    return true;
}

std::string cIniFile::getStr(const std::string& section, const std::string& key) const
{
    if (m_mapConfig.find(section) != m_mapConfig.end()) {
        return m_mapConfig.at(section).getValue(key);
    } else {
        std::cout << " getStr section " << section << " didn't exist" << std::endl;
        return std::string();
    }
}

template<typename T> T cIniFile::FromString(const std::string& value) const
{
    std::istringstream ss(value);
    T res;
    ss >> res;
    return res;
}

int cIniFile::getInt(const std::string& section, const std::string& key) const
{
    return FromString<int>(getStr(section, key));
}

double cIniFile::getDouble(const std::string& section, const std::string& key) const
{
    return FromString<double>(getStr(section, key));
}

bool cIniFile::getBoolean(const std::string& section, const std::string& key) const
{
    std::string value = getStr(section, key);
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c){ return std::tolower(c); });
    if (value == "on" || value== "1" || value == "true")
        return true;

    return false;    
}

std::list<std::string> cIniFile::getSectionsFromIni() const
{
    std::list<std::string> mList;
    for (auto& x: m_mapConfig) {
        mList.push_back(x.first);
    }
    return mList;
}


std::list<std::string> cIniFile::getKeyFromSection(const std::string& section ) const
{
    std::list<std::string> mList;
    if (m_mapConfig.find(section) != m_mapConfig.end())
        mList = m_mapConfig.at(section).getAllKey();
    return mList;    
}