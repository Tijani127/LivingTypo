#include "Particle.hpp"
#include <cmath>

void Particle::update(float dt, sf::Vector2f mouse, float mouse_radius, float k, float damping) {
    sf::Vector2f spring = (target - pos) * k;
    sf::Vector2f damp = vel * damping;

    sf::Vector2f to_mouse = pos - mouse;
    float dist = std::sqrt(to_mouse.x * to_mouse.x + to_mouse.y * to_mouse.y);
    sf::Vector2f mouse_f(0, 0);
    if (dist < mouse_radius && dist > 1.0f) {
        mouse_f = to_mouse / (dist * dist) * 800.0f;
    }

    sf::Vector2f accel = spring - damp + mouse_f;
    vel += accel * dt;
    pos += vel * dt;
}
