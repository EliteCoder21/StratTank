#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <list>

enum class GameState {
    Playing,
    Paused,
    Victory,
    Defeat
};

enum class EntityType {
    Player,
    Ally,
    EnemyLight,
    EnemyHeavy,
    Fort,
    ProjectilePlayer,
    ProjectileEnemy
};

class Entity {
public:
    Entity(float x, float y, EntityType type);
    virtual ~Entity() = default;

    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0;

    sf::Vector2f getPosition() const { return position; }
    sf::Vector2f getSize() const { return size; }
    EntityType getType() const { return type; }
    float getRotation() const { return rotation; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    bool isAlive() const { return health > 0; }
    bool isMarkedForDeletion() const { return markedForDeletion; }

    void setPosition(float x, float y);
    void setRotation(float rot);
    virtual void takeDamage(int damage);
    void heal(int amount);
    void markForDeletion() { markedForDeletion = true; }
    void setSize(sf::Vector2f s) { size = s; }
    void setHealth(int h) { health = h; }
    void setMaxHealth(int mh) { maxHealth = mh; }

    sf::FloatRect getBounds() const;

protected:
    sf::Vector2f position;
    sf::Vector2f size;
    float rotation = 0.0f;
    int health;
    int maxHealth;
    EntityType type;
    bool markedForDeletion = false;
    bool isPlayer = false;
};