#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>
#include "cParticle.h"

class cParticles {
public:
    cParticles()
    {}

    // accès
    cParticle &operator[](std::size_t i) {
        if (i >= MAX_PARTICLES) throw std::out_of_range("Particle index");
        return m_particles[i];
    }

    const cParticle &operator[](std::size_t i) const {
        if (i >= MAX_PARTICLES) throw std::out_of_range("Particle index");
        return m_particles[i];
    }

    // getter explicite
    cParticle &get(std::size_t i) { return (*this)[i]; }
    const cParticle &get(std::size_t i) const { return (*this)[i]; }

    // taille
    static constexpr std::size_t size() noexcept { return MAX_PARTICLES; }

    // recherche/existence
    bool valid(std::size_t i) const noexcept { return i < MAX_PARTICLES; }

    // initialisation
    void reset() noexcept {
        for (auto &p : m_particles) {
            p.reset(); // ou init spécifique
        }
    }

    // itérateurs
    auto begin() noexcept { return m_particles.begin(); }
    auto end() noexcept { return m_particles.end(); }
    auto begin() const noexcept { return m_particles.begin(); }
    auto end() const noexcept { return m_particles.end(); }

private:
    static constexpr std::size_t MAX_PARTICLES = 600;
    std::array<cParticle, MAX_PARTICLES> m_particles;
};