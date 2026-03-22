/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#include "cUnits.h"
#include "cUnit.h"
#include "utils/cLog.h"
#include <format>

cUnits::cUnits() {
    // Units will be initialized through default constructors of std::array
}

cUnit& cUnits::operator[](int index) {
    if (index < 0 || index >= MAX_UNITS_CAPACITY) {
        cLogger::getInstance()->log(LOG_ERROR, eLogComponent::COMP_GAMEOBJECTS, "cUnits::operator[]", 
            std::format("Invalid unit index: {}", index));
        return m_units[0];  // Return first unit as fallback
    }
    return m_units[index];
}

const cUnit& cUnits::operator[](int index) const {
    if (index < 0 || index >= MAX_UNITS_CAPACITY) {
        cLogger::getInstance()->log(LOG_ERROR, eLogComponent::COMP_GAMEOBJECTS, "cUnits::operator[] const", 
            std::format("Invalid unit index: {}", index));
        return m_units[0];  // Return first unit as fallback
    }
    return m_units[index];
}

cUnit* cUnits::getUnit(int index) {
    if (index < 0 || index >= MAX_UNITS_CAPACITY) {
        return nullptr;
    }
    return &m_units[index];
}

const cUnit* cUnits::getUnit(int index) const {
    if (index < 0 || index >= MAX_UNITS_CAPACITY) {
        return nullptr;
    }
    return &m_units[index];
}

int cUnits::findAvailableSlot() const {
    for (int i = 0; i < MAX_UNITS_CAPACITY; i++) {
        if (!m_units[i].isValid()) {
            return i;
        }
    }
    return -1; // No available slot
}

int cUnits::getValidUnitsCount() const {
    int count = 0;
    for (int i = 0; i < MAX_UNITS_CAPACITY; i++) {
        if (m_units[i].isValid()) {
            count++;
        }
    }
    return count;
}
