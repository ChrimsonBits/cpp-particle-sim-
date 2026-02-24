// Copyright (c) 2026 Aschwyn
// Licensed under the MIT License. See LICENSE file in the project root.

#include "particle_sim.hpp"
#include <cmath>
#include <algorithm>

namespace ps {

double length(Vec2 v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

Vec2 normalized(Vec2 v) {
    const double len = length(v);
    if (len <= 1e-12) return {0.0, 0.0};
    return {v.x / len, v.y / len};
}

sf::Color colorFor(ParticleType type) {
    switch (type) {
        case ParticleType::Proton:   return sf::Color::Red;
        case ParticleType::Neutron:  return sf::Color::Blue;
        case ParticleType::Electron: return sf::Color::Yellow;
    }
    return sf::Color::White;
}

Simulation::Simulation(SimConfig cfg)
: m_cfg(cfg) {}

void Simulation::addParticle(Vec2 pos, ParticleType type) {
    Particle p;
    p.pos = pos;
    p.type = type;
    m_particles.push_back(p);
}

double Simulation::forceBetween(const Particle& a, const Particle& b) const {
    // Similar to your original logic, but tidied.
    const Vec2 d{b.pos.x - a.pos.x, b.pos.y - a.pos.y};
    const double r2 = d.x * d.x + d.y * d.y;

    if (r2 <= 1e-9) return m_cfg.forceStrength * 1000.0;

    const double invR2 = 1.0 / r2;
    const double strengthFactor = m_cfg.forceStrength * invR2;

    double force = 0.0;
    if (a.type == ParticleType::Electron) {
        if (b.type == ParticleType::Electron) force = -strengthFactor;
        else if (b.type == ParticleType::Proton) force = strengthFactor;
        else force = strengthFactor * m_cfg.neutronFactor;
    } else if (a.type == ParticleType::Proton) {
        if (b.type == ParticleType::Electron) force = strengthFactor;
        else if (b.type == ParticleType::Proton) force = -strengthFactor;
        else force = strengthFactor * m_cfg.neutronFactor;
    } else { // Neutron
        force = strengthFactor * m_cfg.neutronFactor;
    }

    const double r = std::sqrt(r2);
    if (r < 0.3) force *= -0.95; // your "close range" flip

    return force;
}

void Simulation::step(double dtSeconds) {
    if (dtSeconds <= 0.0) return;

    // Snapshot for consistent interactions
    const auto snapshot = m_particles;

    for (std::size_t i = 0; i < m_particles.size(); ++i) {
        auto& p = m_particles[i];

        // Drag
        p.vel *= m_cfg.drag;

        // Accumulate forces
        p.acc = {0.0, 0.0};

        for (std::size_t j = 0; j < snapshot.size(); ++j) {
            if (i == j) continue;

            const auto& other = snapshot[j];
            const Vec2 d{other.pos.x - p.pos.x, other.pos.y - p.pos.y};
            const double r = length(d);
            if (r <= 1e-12) continue;

            const double f = forceBetween(p, other);
            const Vec2 dir{d.x / r, d.y / r};
            const Vec2 accel{dir.x * f, dir.y * f};

            const double massScale =
                (p.type == ParticleType::Electron) ? m_cfg.electronMassScale : m_cfg.heavyMassScale;

            p.acc.x += accel.x / massScale;
            p.acc.y += accel.y / massScale;
        }

        // Clamp acceleration
        const double aMag = length(p.acc);
        if (aMag > m_cfg.maxAcceleration) {
            const Vec2 aN = normalized(p.acc);
            p.acc = {aN.x * m_cfg.maxAcceleration, aN.y * m_cfg.maxAcceleration};
        }

        // Integrate (semi-implicit Euler)
        p.vel.x += p.acc.x * dtSeconds;
        p.vel.y += p.acc.y * dtSeconds;

        // Clamp velocity
        const double vMag = length(p.vel);
        if (vMag > m_cfg.maxVelocity) {
            const Vec2 vN = normalized(p.vel);
            p.vel = {vN.x * m_cfg.maxVelocity, vN.y * m_cfg.maxVelocity};
        }

        p.pos.x += p.vel.x * dtSeconds;
        p.pos.y += p.vel.y * dtSeconds;
    }
}

void Simulation::constrainToBox(double minX, double minY, double maxX, double maxY) {
    for (auto& p : m_particles) {
        if (p.pos.x < minX) { p.pos.x = minX; p.vel.x = -p.vel.x * m_cfg.bounceLoss; }
        if (p.pos.x > maxX) { p.pos.x = maxX; p.vel.x = -p.vel.x * m_cfg.bounceLoss; }
        if (p.pos.y < minY) { p.pos.y = minY; p.vel.y = -p.vel.y * m_cfg.bounceLoss; }
        if (p.pos.y > maxY) { p.pos.y = maxY; p.vel.y = -p.vel.y * m_cfg.bounceLoss; }
    }
}

} // namespace ps
