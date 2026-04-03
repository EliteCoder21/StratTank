#pragma once

#include "Entity.h"

class Projectile : public Entity {
public:
    Projectile(float x, float y, float angle, int damage, bool isPlayerProjectile);

    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

    int getDamage() const { return damage; }
    bool isPlayerProjectile() const { return playerProjectile; }

    void setDirection(sf::Vector2f dir);
    sf::Vector2f getDirection() const { return direction; }

private:
    float speed = 400.0f;
    int damage;
    bool playerProjectile;
    sf::Vector2f direction;
    float lifetime;
};