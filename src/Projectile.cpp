#include "Projectile.h"
#include "WorldConfig.h"
#include <cmath>

Projectile::Projectile(float x, float y, float angle, int dmg, bool isPlayerProj)
    : Entity(x, y, isPlayerProj ? EntityType::ProjectilePlayer : EntityType::ProjectileEnemy),
      damage(dmg), playerProjectile(isPlayerProj) {
    size = {8, 8};
    
    float rad = angle * 3.14159f / 180.0f;
    direction = {std::cos(rad), std::sin(rad)};
    
    lifetime = std::hypot(WorldConfig::WIDTH, WorldConfig::HEIGHT) / speed;
}

void Projectile::update(float deltaTime) {
    position.x += direction.x * speed * deltaTime;
    position.y += direction.y * speed * deltaTime;
    
    if (position.x < 0) position.x = 0;
    if (position.y < 0) position.y = 0;
    if (position.x > WorldConfig::WIDTH) position.x = WorldConfig::WIDTH;
    if (position.y > WorldConfig::HEIGHT) position.y = WorldConfig::HEIGHT;
    
    lifetime -= deltaTime;
    if (lifetime <= 0) {
        markForDeletion();
    }
    
    if (position.x <= 0 || position.x >= WorldConfig::WIDTH || position.y <= 0 || position.y >= WorldConfig::HEIGHT) {
        markForDeletion();
    }
}

void Projectile::render(sf::RenderWindow& window) {
    sf::CircleShape shape(4.0f);
    shape.setOrigin({4.0f, 4.0f});
    shape.setPosition(position);
    shape.setFillColor(playerProjectile ? sf::Color(255, 215, 0) : sf::Color(255, 69, 0));
    window.draw(shape);
}

void Projectile::setDirection(sf::Vector2f dir) {
    direction = dir;
}