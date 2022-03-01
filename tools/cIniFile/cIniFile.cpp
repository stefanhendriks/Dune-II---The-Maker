#include "cIniFile.h"
#include <fstream>
#include <iostream>

using namespace std;


static void Trim(std::string& str)
{
    str.erase(str.find_last_not_of(" \t")+1);         //suffixing spaces
    str.erase(0, str.find_first_not_of(" \t"));       //prefixing spaces
   // return str;
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
        std::cout << "Key already exist" << std::endl;
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
        std::cout << "key didn't exist" << std::endl;
        return string();
    }
}

//
// cIniFile class
//

cIniFile::cIniFile(const std::string &configFileName)
    :m_fileName(configFileName){
    load(m_fileName);
}

cIniFile::~cIniFile(void) { }


bool cIniFile::load(const string& config)
{
    m_fileName = config;
    std::ifstream in(m_fileName.c_str());
    if (!in) {
        std::cout << "unable to open file" << std::endl;
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
                std::cout << "section already exist" << std::endl;
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
        if (m_actualSection.empty()) {
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
    if (sec_begin_pos == string::npos || sec_begin_pos != 0){
        return false;
    }
    size_t sec_end_pos = inputLine.find(']', sec_begin_pos);
    if (sec_end_pos == string::npos){
        return false;
    }

    m_actualSection = (inputLine.substr(sec_begin_pos + 1, sec_end_pos - sec_begin_pos - 1));
    Trim(inputLine);
    return true;
}

bool cIniFile::isKeyValue(string inputLine)
{
    size_t keyPos = inputLine.find('=');
    if (keyPos == string::npos || keyPos == 0 || keyPos == inputLine.length()-1 ){
        m_actualKey.clear();
        m_actualValue.clear();
        return false;
    }
    string key = inputLine.substr(0, keyPos);
    Trim(key);
    m_actualKey = key;

    string value = inputLine.substr(keyPos+1);
    Trim(value);
    m_actualValue = value;

    if (m_actualKey.empty() || m_actualValue.empty()) {
        m_actualKey.clear();
        m_actualValue.clear();
        return false;
    }
    return true;
}

bool cIniFile::save(const std::string& savepath)
{
    ofstream ou;
    if (savepath.empty()) {
        ou.open(m_fileName.c_str());
    }
    else {
        ou.open(savepath.c_str());
    }
    ou.clear();
    /*for (std::string i: m_order_sec)
        std::cout << i << ' ';
	for (int i = 0; i < m_order_sec.size(); i++){
		std::string secname = m_order_sec[i];
		Section& sec = m_mapConfig[secname];
		ou << "[" << secname << "]" << endl;
		map<string, KeyVal> data = sec.GetData();

		for (int j = 0; j < sec.m_key_order.size(); j++) {
			std::string key = sec.m_key_order[j];

			std::vector<std::string> vprcomm = sec.m_pre_comment[key];
			std::vector<std::string> vnextcomm = sec.m_next_comment[key];
			if (vprcomm.size() > 0 ) {
				for (int k = 0; k < vprcomm.size(); k++){
					ou << vprcomm[k] << std::endl;
				}
			}
			ou << key << " = " << data[key].Get() << endl;
			if (vnextcomm.size() > 0 ) {
				for (int k = 0; k < vnextcomm.size(); k++){
					ou << vnextcomm[k] << std::endl;
				}
			}
		}
		ou << endl;
	}*/
    ou.close();
    return true;
}


std::string cIniFile::getStr(const std::string& section, const std::string& key) const
{
    if (m_mapConfig.find(section) != m_mapConfig.end()) {
        return m_mapConfig.at(section).getValue(key);
    } else {
        std::cout << "section didn't exist" << std::endl;
        return string();
    }
}