#pragma once

#include "Entity.h"
#include <memory>
#include <functional>

class Entity;

class Fort : public Entity {
public:
    Fort(float x, float y);

    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

    void setPlayerPosition(sf::Vector2f pos);
    void setAllyTargets(const std::vector<Entity*>& allies);
    void clearAllyTargets() { allyTargets.clear(); }
    void addEnemyTarget(Entity* enemy) { allyTargets.push_back(enemy); }
    
    void setProjectileCallback(std::function<void(float, float, float, int, bool)> callback) {
        projectileCallback = callback;
    }
    
    void setBombCallback(std::function<void(float, float, float)> callback) {
        bombCallback = callback;
    }
    void setShowHealthBar(bool show) { showHealthBar = show; }

    void shoot();
    bool canShoot() const;
    void updateCooldown(float deltaTime);
    
    bool canBomb() const { return bombCooldown <= 0.0f && !bombTargets.empty(); }
    void triggerBomb();

private:
    float turretRotation = 0.0f;
    int damage = 25;
    float fireRate = 1.5f;
    float fireCooldown = 0.0f;
    sf::Vector2f playerPosition;
    std::vector<Entity*> allyTargets;
    float rotationSpeed = 1.0f;
    std::function<void(float, float, float, int, bool)> projectileCallback;
    std::function<void(float, float, float)> bombCallback;
    float bombCooldown = 8.0f;
    int bombDamage = 500;
    float bombRadius = 400.0f;
    std::vector<Entity*> bombTargets;
    bool showHealthBar = true;
    sf::Vector2f bombTargetPosition = {0, 0};
    bool bombWarningActive = false;

    void updateAI();
};