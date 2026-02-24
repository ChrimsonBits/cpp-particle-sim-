// Copyright (c) 2026 Aschwyn
// Licensed under the MIT License. See LICENSE file in the project root.

#include <SFML/Graphics.hpp>
#include <optional>
#include "particle_sim.hpp"

int main() {
    // Create a resizable window
    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u(1280, 720), 32),
                            "Particle Simulation",
                            sf::Style::Default
    );
    window.setFramerateLimit(60);

    // Simulation + render mapping parameters
    const float scale = 50.0f;

    float centerX = 0.f;
    float centerY = 0.f;

    // Simulation bounds in simulation-space
    double simMinX = 0.0, simMaxX = 0.0, simMinY = 0.0, simMaxY = 0.0;

    // Your sim object (adjust type/name to match your code)
    // If you followed the refactor I suggested earlier:
    // ps::Simulation sim;
    //
    // If your current project has a different class, swap this accordingly.
    ps::Simulation sim;

    auto recomputeMapping = [&]() {
        const auto size = window.getSize();

        // Keep view 1:1 with pixels so resizing doesn't stretch the scene
        window.setView(sf::View(sf::FloatRect(
            {0.f, 0.f},
            {static_cast<float>(size.x), static_cast<float>(size.y)}
        )));

        // Center in pixels
        centerX = static_cast<float>(size.x) * 0.5f;
        centerY = static_cast<float>(size.y) * 0.5f;

        // Bounds in sim-space (derived from pixel size and scale)
        simMinX = (0.0 - centerX) / scale;
        simMaxX = (static_cast<double>(size.x) - centerX) / scale;
        simMinY = (0.0 - centerY) / scale;
        simMaxY = (static_cast<double>(size.y) - centerY) / scale;
    };

    recomputeMapping();

    // Fixed timestep physics
    sf::Clock clock;
    double accumulator = 0.0;
    constexpr double dt = 1.0 / 120.0;

    while (window.isOpen()) {
        // Events
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            // Resize handling (update view + bounds)
            if (event->is<sf::Event::Resized>()) {
                recomputeMapping();
            }

            if (const sf::Event::KeyPressed* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvent->code == sf::Keyboard::Key::P ||
                    keyEvent->code == sf::Keyboard::Key::N ||
                    keyEvent->code == sf::Keyboard::Key::E) {

                    const sf::Vector2i pixelPos = sf::Mouse::getPosition(window);

                const double simX = (pixelPos.x - centerX) / scale;
                const double simY = (pixelPos.y - centerY) / scale;

                ps::ParticleType type = ps::ParticleType::Proton;
                if (keyEvent->code == sf::Keyboard::Key::N) type = ps::ParticleType::Neutron;
                if (keyEvent->code == sf::Keyboard::Key::E) type = ps::ParticleType::Electron;

                // Adjust this to your sim API
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

            // Keep particles inside the resized window bounds
            sim.constrainToBox(simMinX, simMinY, simMaxX, simMaxY);

            accumulator -= dt;
        }

        // Render
        window.clear();

        for (const auto& p : sim.particles()) {
            const float x = centerX + static_cast<float>(p.pos.x) * scale;
            const float y = centerY + static_cast<float>(p.pos.y) * scale;

            sf::CircleShape circle(5.0f);
            circle.setOrigin(sf::Vector2f(5.0f, 5.0f));
            circle.setPosition(sf::Vector2f(x, y));
            circle.setFillColor(colorFor(p.type)); // or colorFor(...) if using refactor
            window.draw(circle);
        }

        window.display();
    }

    return 0;
}
