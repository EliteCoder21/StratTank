#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    float lifetime;
    float maxLifetime;
    float size;
};

class ParticleSystem {
public:
    ParticleSystem();

    void emitExplosion(sf::Vector2f position);
    void emitMuzzleFlash(sf::Vector2f position, sf::Vector2f direction);
    void emitSmoke(sf::Vector2f position);

    void update(float deltaTime);
    void render(sf::RenderWindow& window);

private:
    std::vector<Particle> particles;
};