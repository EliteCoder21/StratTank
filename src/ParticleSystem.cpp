#include "ParticleSystem.h"
#include <cmath>

ParticleSystem::ParticleSystem() {}

void ParticleSystem::emitExplosion(sf::Vector2f position) {
    for (int i = 0; i < 20; i++) {
        float angle = static_cast<float>(rand() % 360) * 3.14159f / 180.0f;
        float speed = 50.0f + static_cast<float>(rand() % 100);
        
        Particle p;
        p.position = position;
        p.velocity = {std::cos(angle) * speed, std::sin(angle) * speed};
        p.color = sf::Color(255, 100 + rand() % 100, 0);
        p.lifetime = 0.5f + static_cast<float>(rand() % 50) / 100.0f;
        p.maxLifetime = p.lifetime;
        p.size = 5.0f + static_cast<float>(rand() % 10);
        
        particles.push_back(p);
    }
}

void ParticleSystem::emitMuzzleFlash(sf::Vector2f position, sf::Vector2f direction) {
    for (int i = 0; i < 5; i++) {
        float spread = 0.3f;
        float angle = std::atan2(direction.y, direction.x) + 
                      (static_cast<float>(rand() % 100) - 50) / 100.0f * spread;
        float speed = 100.0f + static_cast<float>(rand() % 50);
        
        Particle p;
        p.position = position;
        p.velocity = {std::cos(angle) * speed, std::sin(angle) * speed};
        p.color = sf::Color(255, 255, 200);
        p.lifetime = 0.1f;
        p.maxLifetime = p.lifetime;
        p.size = 3.0f;
        
        particles.push_back(p);
    }
}

void ParticleSystem::emitSmoke(sf::Vector2f position) {
    for (int i = 0; i < 8; i++) {
        float angle = static_cast<float>(rand() % 360) * 3.14159f / 180.0f;
        float speed = 20.0f + static_cast<float>(rand() % 30);
        
        Particle p;
        p.position = position;
        p.velocity = {std::cos(angle) * speed, std::sin(angle) * speed - 20.0f};
        p.color = sf::Color(100, 100, 100, 200);
        p.lifetime = 0.8f + static_cast<float>(rand() % 40) / 100.0f;
        p.maxLifetime = p.lifetime;
        p.size = 4.0f + static_cast<float>(rand() % 6);
        
        particles.push_back(p);
    }
}

void ParticleSystem::update(float deltaTime) {
    for (auto it = particles.begin(); it != particles.end();) {
        it->lifetime -= deltaTime;
        it->position += it->velocity * deltaTime;
        it->velocity *= 0.95f;
        
        if (it->lifetime <= 0) {
            it = particles.erase(it);
        } else {
            ++it;
        }
    }
}

void ParticleSystem::render(sf::RenderWindow& window) {
    for (const auto& p : particles) {
        float alpha = p.lifetime / p.maxLifetime;
        sf::CircleShape shape(p.size * alpha);
        shape.setOrigin({p.size * alpha, p.size * alpha});
        shape.setPosition(p.position);
        
        sf::Color c = p.color;
        c.a = static_cast<std::uint8_t>(255 * alpha);
        shape.setFillColor(c);
        
        window.draw(shape);
    }
}