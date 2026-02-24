// Copyright (c) 2026 Aschwyn
// Licensed under the MIT License. See LICENSE file in the project root.

#include <SFML/Graphics.hpp>
#include <optional>
#include "particle_sim.hpp"

int main() {
    constexpr unsigned WIN_W = 1280;
    constexpr unsigned WIN_H = 720;

    sf::RenderWindow window(sf::VideoMode({WIN_W, WIN_H}), "Particle Simulation");
    window.setFramerateLimit(60);

    // Render mapping
    const float scale = 50.0f;
    const float centerX = WIN_W * 0.5f;
    const float centerY = WIN_H * 0.5f;

    // Simulation box in sim-space coordinates (derived from window size)
    const double simMinX = (0.0 - centerX) / scale;
    const double simMaxX = (double(WIN_W) - centerX) / scale;
    const double simMinY = (0.0 - centerY) / scale;
    const double simMaxY = (double(WIN_H) - centerY) / scale;

    ps::Simulation sim;

    // Fixed timestep loop
    sf::Clock clock;
    double accumulator = 0.0;
    constexpr double dt = 1.0 / 120.0; // physics tick

    while (window.isOpen()) {
        // Events
        while (auto ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) window.close();

            if (const auto* key = ev->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::P ||
                    key->code == sf::Keyboard::Key::N ||
                    key->code == sf::Keyboard::Key::E) {

                    const sf::Vector2i pixel = sf::Mouse::getPosition(window);
                    const double simX = (pixel.x - centerX) / scale;
                    const double simY = (pixel.y - centerY) / scale;

                    ps::ParticleType type = ps::ParticleType::Proton;
                    if (key->code == sf::Keyboard::Key::N) type = ps::ParticleType::Neutron;
                    if (key->code == sf::Keyboard::Key::E) type = ps::ParticleType::Electron;

                    sim.addParticle({simX, simY}, type);
                }
            }
        }

        // Timing
        const double frameTime = clock.restart().asSeconds();
        accumulator += frameTime;

        // Physics steps
        while (accumulator >= dt) {
            sim.step(dt);
            sim.constrainToBox(simMinX, simMinY, simMaxX, simMaxY);
            accumulator -= dt;
        }

        // Render
        window.clear();
        for (const auto& p : sim.particles()) {
            const float x = centerX + float(p.pos.x) * scale;
            const float y = centerY + float(p.pos.y) * scale;

            sf::CircleShape circle(5.0f);
            circle.setOrigin({5.0f, 5.0f});
            circle.setPosition({x, y});
            circle.setFillColor(ps::colorFor(p.type));
            window.draw(circle);
        }
        window.display();
    }

    return 0;
}
