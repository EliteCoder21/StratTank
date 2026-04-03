#pragma once

#include "Entity.h"
#include "WorldConfig.h"
#include <cmath>
#include <functional>

class Tank : public Entity {
public:
    Tank(float x, float y, EntityType type, int maxHealth, float speed, int damage, float fireRate);

    virtual void update(float deltaTime) override;
    virtual void render(sf::RenderWindow& window) override;

    void setTurretRotation(float rot) { turretRotation = rot; }
    float getTurretRotation() const { return turretRotation; }

    float getSpeed() const { return speed; }
    int getDamage() const { return damage; }
    float getFireRate() const { return fireRate; }
    float getFireCooldown() const { return fireCooldown; }

    void shoot();
    bool canShoot() const;
    void updateCooldown(float deltaTime);
    
    void setBarriers(const std::vector<sf::FloatRect>* barriers) { this->barriers = barriers; }
    bool checkBarrierCollision(const sf::Vector2f& newPos);
    bool isInsideBarrier();
    void teleportToRandomPosition();
    sf::Vector2f getSteeringFromObstacles(const sf::Vector2f& direction);
    
    void setProjectileCallback(std::function<void(float, float, float, int, bool)> callback) {
        projectileCallback = callback;
    }

    virtual void move(float dx, float deltaTime);
    virtual void setTargetPosition(sf::Vector2f target);

protected:
    float speed;
    int damage;
    float fireRate;
    float fireCooldown = 0.0f;
    float turretRotation = 0.0f;
    sf::Vector2f targetPosition;
    bool hasTarget = false;
    std::function<void(float, float, float, int, bool)> projectileCallback;
    const std::vector<sf::FloatRect>* barriers = nullptr;

    virtual void renderBody(sf::RenderWindow& window);
    virtual void renderTurret(sf::RenderWindow& window);
};