#pragma once

class cPlayerStatistics {
public:
    cPlayerStatistics();

    // set methods
    void setEnemyUnitsDestroyed(int theEnemyUnitsDestroyed) {
        m_enemyUnitsDestroyed = theEnemyUnitsDestroyed;
    }
    void setEnemyStructuresDestroyed(int theEnemyStructuresDestroyed) {
        m_enemyStructuresDestroyed = theEnemyStructuresDestroyed;
    }
    void setUnitsDestroyed(int theUnitsDestroyed) {
        m_unitsDestroyed = theUnitsDestroyed;
    }
    void setUnitsEaten(int theUnitsEaten) {
        m_unitsEaten = theUnitsEaten;
    }
    void setStructuresDestroyed(int theStructuresDestroyed) {
        m_structuresDestroyed = theStructuresDestroyed;
    }
    void setSpiceMined(int theSpiceMined) {
        m_spiceMined = theSpiceMined;
    }
    void setStructuresConstructed(int theStructuresConstructed) {
        m_structuresConstructed = theStructuresConstructed;
    }
    void setUnitsConstructed(int theUnitsConstructed) {
        m_unitsConstructed = theUnitsConstructed;
    }
    void setSuperWeaponsUsed(int theSuperWeaponsUsed) {
        m_superWeaponsUsed = theSuperWeaponsUsed;
    }

    // get methods
    int getEnemyUnitsDestroyed() {
        return m_enemyUnitsDestroyed;
    }
    int getEnemyStructuresDestroyed() {
        return m_enemyStructuresDestroyed;
    }
    int getUnitsDestroyed() {
        return m_unitsDestroyed;
    }
    int getUnitsEaten() {
        return m_unitsEaten;
    }
    int getStructuresDestroyed() {
        return m_structuresDestroyed;
    }
    int getSpiceMined() {
        return m_spiceMined;
    }
    int getStructuresConstructed() {
        return m_structuresConstructed;
    }
    int getUnitsConstructed() {
        return m_unitsConstructed;
    }
    int getSuperWeaponsUsed() {
        return m_superWeaponsUsed;
    }

private:
    // enemy units and structures destroyed
    int m_enemyUnitsDestroyed;
    int m_enemyStructuresDestroyed;

    // casualties
    int m_unitsDestroyed;	// unit has been destroyed by (enemy) fire
    int m_unitsEaten; // unit has been eaten by sandworm
    int m_structuresDestroyed; // structure has been destroyed

    // misc
    int m_spiceMined;	// total amount of spice mined
    int m_structuresConstructed; // total amount of structures constructed
    int m_unitsConstructed; // total amount of units constructed
    int m_superWeaponsUsed; // total amount of super weapons used
};
