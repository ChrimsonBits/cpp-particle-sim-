#pragma once

// Copyright (c) 2026 Aschwyn
// Licensed under the MIT License. See LICENSE file in the project root.

#include <SFML/Graphics/Color.hpp>
#include <vector>
#include <cstdint>

namespace ps {

struct Vec2 {
    double x = 0.0;
    double y = 0.0;

    Vec2() = default;
    Vec2(double x_, double y_) : x(x_), y(y_) {}

    Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
    Vec2& operator-=(const Vec2& o) { x -= o.x; y -= o.y; return *this; }
    Vec2& operator*=(double s) { x *= s; y *= s; return *this; }
};

inline Vec2 operator+(Vec2 a, const Vec2& b) { a += b; return a; }
inline Vec2 operator-(Vec2 a, const Vec2& b) { a -= b; return a; }
inline Vec2 operator*(Vec2 a, double s) { a *= s; return a; }

double length(Vec2 v);
Vec2 normalized(Vec2 v);

enum class ParticleType : std::uint8_t { Proton, Neutron, Electron };

sf::Color colorFor(ParticleType type);

struct Particle {
    Vec2 pos{};
    Vec2 vel{};
    Vec2 acc{};
    ParticleType type = ParticleType::Proton;
};

struct SimConfig {
    double forceStrength = 0.1;
    double drag = 0.99;
    double maxAcceleration = 0.1;
    double maxVelocity = 0.5;

    // Soft boundary "bounce"
    double bounceLoss = 0.8;

    // Neutron / other scaling
    double neutronFactor = 0.1;

    // Proton/electron mass scaling
    double electronMassScale = 1.0;
    double heavyMassScale = 1836.0;
};

class Simulation {
public:
    explicit Simulation(SimConfig cfg = {});

    void addParticle(Vec2 pos, ParticleType type);
    void step(double dtSeconds);

    const std::vector<Particle>& particles() const { return m_particles; }

    // Constrain within a rectangle in *simulation space*
    void constrainToBox(double minX, double minY, double maxX, double maxY);

private:
    double forceBetween(const Particle& a, const Particle& b) const;

    SimConfig m_cfg{};
    std::vector<Particle> m_particles{};
};

} // namespace ps
