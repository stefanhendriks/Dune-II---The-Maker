/**
 * @file cGameObjectContext.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include <stdexcept>
#include <cstddef>
#include <memory>
#include "gameobjects/map/MapGeometry.hpp"

class cBullets;
class cPlayers;
class cParticles;
class cStructures;
class cUnits;
class cMap;
class cPreviewMaps;
class cPlayer;
class cUnit;
class cAbstractStructure;
class cStructureFactory;

struct sGameServices;

class cGameObjectContext {
public:
    cGameObjectContext(
        std::unique_ptr<cBullets> bullets,
        std::unique_ptr<cPlayers> players,
        std::unique_ptr<cParticles> particles,
        std::unique_ptr<cStructures> structures,
        std::unique_ptr<cUnits> units,
        std::unique_ptr<cMap> map,
        std::unique_ptr<cPreviewMaps> previewMaps,
        std::unique_ptr<cStructureFactory> structureFactory);
    ~cGameObjectContext();

    cBullets& getBullets() const;
    cMap* getMap() const;
    MapGeometry* getMapGeometry() const;
    cPlayers* getPlayers() const;
    cParticles& getParticles() const;

    cStructures& getStructures() const;
    cAbstractStructure* getStructure(int index);
    cPreviewMaps* getPreviewMaps() const;

    cUnits* getUnits() const;
    int getUnitsSize() const;
    cUnit* getUnit(std::size_t index);
    cUnit* getUnit(int index);

    cPlayer* getPlayer(int index);
    const cPlayer* getPlayer(int index) const;

    cStructureFactory* getStructureFactory() const;

    void serviceInit(sGameServices* services);
private:
    std::unique_ptr<cBullets> m_Bullets;
    std::unique_ptr<cPlayers> m_Players;
    std::unique_ptr<cParticles> m_particles;
    std::unique_ptr<cStructures> m_pStructures;
    std::unique_ptr<cUnits> m_Units;
    std::unique_ptr<cMap> m_map;
    std::unique_ptr<cPreviewMaps> m_previewMaps;
    std::unique_ptr<cStructureFactory> m_structureFactory;
};
