#include "cIniFile.h"
#include "cLog.h"
#include <fstream>
#include <fmt/core.h>
#include <algorithm>

static void Trim(std::string &str) {
    str.erase(str.find_last_not_of(" \t") + 1);       // suffixing spaces
    str.erase(0, str.find_first_not_of(" \t"));       // prefixing spaces
}

//
// cSection class
//

cSection::cSection() {}

cSection::~cSection() {
    m_sectionConf.clear();
    m_dataConfs.clear();
}

cSection::cSection(const std::string &secName) : m_sectionName(secName) {}

bool cSection::addValue(const std::string &key, const std::string &value) {
    if (m_sectionConf.find(value) != m_sectionConf.end()) {
        //std::cout << "Key " << key << " already exist on section " << m_sectionName << std::endl;
        cLogger *logger = cLogger::getInstance();
        logger->log(LOG_WARN, COMP_GAMEINI, "(cSection)",
                    fmt::format("Key {} already exist on section {}", key, m_sectionName));
        return false;
    }
    m_sectionConf[key] = value;
    return true;
}

bool cSection::addData(const std::string &data) {
    m_dataConfs.push_back(data);
    return true;
}


/**
 * Returns value as string from section given a 'key'. Ie, suppose this is a section [SETTINGS] and there
 * is something defined like 'screenWidth=123', then you provide as key 'screenWidth', and this function
 * will return `123` as string.
 * @param key
 * @return
 */
std::string cSection::getStringValue(const std::string &key) const {
    if (m_sectionConf.find(key) != m_sectionConf.end()) {
        return m_sectionConf.at(key);
    } else {
        //std::cout << "key " << key << " didn't exist on section " << m_sectionName << std::endl;
        cLogger *logger = cLogger::getInstance();
        logger->log(LOG_WARN, COMP_GAMEINI, "(cSection)",
                    fmt::format("Key {} didn't exist on section {}", key, m_sectionName));
        return std::string();
    }
}

template<typename T>
T cSection::FromString(std::string value) const {
    std::istringstream ss(value);
    T res;
    ss >> res;
    return res;
}

int cSection::getInt(const std::string &key) const {
    const std::string &value = getStringValue(key);
    if (!value.empty()) {
        return FromString<int>(value);
    }

    return 0;
}

bool cSection::getBoolean(const std::string &key) const {
    std::string value = getStringValue(key);
    if (!value.empty()) {
        std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return std::tolower(c); });

        if (value == "on" || value == "1" || value == "true")
            return true;
    }

    return false;
}

double cSection::getDouble(const std::string &key) const {
    const std::string &value = getStringValue(key);
    if (!value.empty()) {
        return FromString<double>(value);
    }

    return 0;
}


//
// cIniFile class
//

cIniFile::cIniFile(const std::string &configFileName)
        : m_fileName(configFileName) {
    load(m_fileName);
}

cIniFile::~cIniFile() {}


bool cIniFile::load(const std::string &config) {
    cLogger *logger = cLogger::getInstance();
    m_fileName = config;
    logger->log(LOG_INFO, COMP_GAMEINI, "(cIniFile)", fmt::format("Load file {}", m_fileName));
    std::ifstream in(m_fileName.c_str());
    if (!in) {
        // std::cout << "unable to open file " << m_fileName << std::endl;
        //cLogger *logger = cLogger::getInstance();
        logger->log(LOG_ERROR, COMP_GAMEINI, "(cIniFile)", fmt::format("Unable to open file Key {}", m_fileName));
        return false;
    }

    std::string line, secName, lastSecName;
    while (!in.eof()) {
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
                logger->log(LOG_WARN, COMP_GAMEINI, "(cIniFile)",
                            fmt::format("section {} already exist", m_actualSection));
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
        logger->log(LOG_WARN, COMP_GAMEINI, "(cIniFile)", fmt::format("Error {} or no section found", line));
    }
    return true;
}

bool cIniFile::isSectionName(std::string inputLine) {
    size_t sec_begin_pos = inputLine.find('[');
    if (sec_begin_pos == std::string::npos || sec_begin_pos != 0) {
        return false;
    }
    size_t sec_end_pos = inputLine.find(']', sec_begin_pos);
    if (sec_end_pos == std::string::npos) {
        return false;
    }

    m_actualSection = (inputLine.substr(sec_begin_pos + 1, sec_end_pos - sec_begin_pos - 1));
    Trim(inputLine);
    return true;
}

bool cIniFile::isKeyValue(std::string inputLine) {
    size_t keyPos = inputLine.find('=');
    if (keyPos == std::string::npos || keyPos == 0 || keyPos == inputLine.length() - 1) {
        m_actualKey.clear();
        m_actualValue.clear();
        return false;
    }
    std::string key = inputLine.substr(0, keyPos);
    Trim(key);
    m_actualKey = key;

    std::string value = inputLine.substr(keyPos + 1);
    Trim(value);
    m_actualValue = value;

    if (m_actualKey.empty() || m_actualValue.empty()) {
        m_actualKey.clear();
        m_actualValue.clear();
        return false;
    }
    return true;
}

/**
 * Returns value of given 'key' and 'section'. Example, given an ini file which contains a [SETTINGS] section
 * and within it has 'foo=345'. Then you could retrieve value of 'foo' by providing section name `SETTINGS` and
 * 'key' = foo. This will return `345` as a string value.
 * <br/><br/>
 * Returns empty string In case a section (or key within an existing session) cannot be found.
 * @param section
 * @param key
 * @return
 */
std::string cIniFile::getStringValue(const std::string &section, const std::string &key) const {
    if (hasSection(section)) {
        return getSection(section).getStringValue(key);
    } else {
        // std::cout << " getStringValue section " << section << " didn't exist" << std::endl;
        cLogger *logger = cLogger::getInstance();
        logger->log(LOG_ERROR, COMP_GAMEINI, "(cIniFile)", fmt::format(" getStringValue section {} didn't exist", section));

        return std::string();
    }
}

bool cIniFile::hasSection(const std::string &section) const { return m_mapConfig.find(section) != m_mapConfig.end(); }


cSection cIniFile::getSection(const std::string &section) const {
    return m_mapConfig.at(section);
}
