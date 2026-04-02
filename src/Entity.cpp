#include "Entity.h"
#include <algorithm>

Entity::Entity(float x, float y, EntityType type)
    : position(x, y), type(type), health(0), maxHealth(0) {}

void Entity::setPosition(float x, float y) {
    position = {x, y};
}

void Entity::setRotation(float rot) {
    rotation = rot;
}

void Entity::takeDamage(int damage) {
    health -= damage;
    if (health <= 0) {
        health = 0;
        markForDeletion();
    }
}

void Entity::heal(int amount) {
    health = std::min(health + amount, maxHealth);
}

sf::FloatRect Entity::getBounds() const {
    return sf::FloatRect(
        {position.x - size.x / 2, position.y - size.y / 2},
        size
    );
}