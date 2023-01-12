#include "Tracy.hpp"
#include "rendering/window.h"
#include "particlesystem/particlesystem.h"

#include <cmath>
#include <cstdlib>

// Simple random functions to get started
// Random float (0,1)
float rnd() {
    return rand() / static_cast<float>(RAND_MAX);
}

// Random float (-1,1)
float srnd() {
    return rnd() * 2.0f - 1.0f;
}

int main(int, char**) {
    rendering::createWindow();

    // --- EXAMPLE SNIPPET ---
    // Create some particle render info which is a simple struct that contains all visual properties used to render particles.
    // This is to exemplify the render[Particles/Emitters/Forces] functions
    std::vector<rendering::ParticleInfo> particles(100);
    for (rendering::ParticleInfo& particle : particles) {
        particle.position = {srnd(), srnd()};     // Position between (-1,1) = Screen extent
        particle.color = {rnd(), rnd(), rnd()};   // Color between (0-1) per channel
        particle.radius = {1.0f + rnd() * 9.0f};  // Radius between (1.0-10.0)
    }
    // --- END EXAMPLE SNIPPET ---

    double t = 0.0;
    float speed = 1.0f;
    bool isRunning = true;
    while (isRunning) {
        const float dt = rendering::beginFrame();
        t += dt;

        particleSystem.update(dt * speed);
        particleSystem.render();

        // --- EXAMPLE SNIPPET ---
        // Create some global smooth rocking motion
        const vec2 vel = vec2(static_cast<float>(std::cos(t * 0.5)), -static_cast<float>(std::abs(std::sin(t * 0.5)))) * 0.2f;
        for (rendering::ParticleInfo& particle : particles) {
            // Apply per particle jitter
            const vec2 jitter = vec2(srnd(), srnd()) * 1.0f;
            particle.position += (vel + jitter) * dt * speed;
            // Check against extent of screen
            if (particle.position.x < -1 || particle.position.x > 1 ||
                particle.position.y < -1 || particle.position.y > 1) {
                // Reset particle if outside
                particle.position = { srnd(), srnd() };
                particle.color = { rnd(), rnd(), rnd() };
                particle.radius = { 1.0f + rnd() * 9.0f };
            }
        }
        rendering::renderParticles(particles);
        // --- END EXAMPLE SNIPPET ---

        {
            ui::GuiScope ui;  // Initiates and finalizes UI rendering upon
                              // construction/destruction
            // @TODO: Replace this example code with your own UI elements
            ui::text("I'm text!");
            ui::sliderFloat("Simulation speed", speed, 0.001f, 10.0f);
            if (ui::button("Close application")) {
                isRunning = false;
            }
        }

        isRunning &= rendering::endFrame();
    }

    rendering::destroyWindow();

    return EXIT_SUCCESS;
}