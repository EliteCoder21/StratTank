#include "Projectile.h"
#include <cmath>

Projectile::Projectile(float x, float y, float angle, int dmg, bool isPlayerProj)
    : Entity(x, y, isPlayerProj ? EntityType::ProjectilePlayer : EntityType::ProjectileEnemy),
      damage(dmg), playerProjectile(isPlayerProj) {
    size = {8, 8};
    
    float rad = angle * 3.14159f / 180.0f;
    direction = {std::cos(rad), std::sin(rad)};
}

void Projectile::update(float deltaTime) {
    position.x += direction.x * speed * deltaTime;
    position.y += direction.y * speed * deltaTime;
    
    lifetime -= deltaTime;
    if (lifetime <= 0) {
        markForDeletion();
    }
    
    if (position.x < 0 || position.x > 2000 || position.y < 0 || position.y > 1500) {
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