#pragma once
#include <SFML/Graphics.hpp>

struct Particle {
    sf::Vector2f pos;
    sf::Vector2f vel;
    sf::Vector2f target;
    char ch = '?';

    void update(float dt, sf::Vector2f mouse, float mouse_radius, float k, float damping);
};
