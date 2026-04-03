#include "AllyTank.h"
#include "Entity.h"
#include "PlayerTank.h"
#include "EnemyTank.h"
#include "WorldConfig.h"
#include <cmath>
#include <algorithm>

AllyTank::AllyTank(float x, float y, int id)
    : Tank(x, y, EntityType::Ally, 60, 120.0f, 12, 3.0f), allyId(id), aiUpdateTimer(0.0f) {
    size = {35, 35};
    autoTargetEnabled = true;
    patrolTarget = position;
}

void AllyTank::update(float deltaTime) {
    Tank::update(deltaTime);
    updateCooldown(deltaTime);
    
    aiUpdateTimer += deltaTime;
    if (aiUpdateTimer < 0.05f) return;
    aiUpdateTimer = 0.0f;
    
    if (attackTarget && attackTarget->isAlive()) {
        attackEntity(attackTarget);
    } else {
        attackTarget = nullptr;
        if (autoTargetEnabled) {
            findAndAttackNearestEnemy();
        } else {
            wander();
        }
    }
}

void AllyTank::findAndAttackNearestEnemy() {
    if (!enemyList || enemyList->empty()) {
        clearTarget();
        wander();
        return;
    }
    
    const float DETECTION_RADIUS = 500.0f;
    
    Entity* nearestEnemy = nullptr;
    float nearestDist = std::numeric_limits<float>::max();
    
    for (auto& enemy : *enemyList) {
        if (enemy && enemy->isAlive()) {
            float dist = std::sqrt(
                std::pow(enemy->getPosition().x - position.x, 2) + 
                std::pow(enemy->getPosition().y - position.y, 2)
            );
            if (dist < DETECTION_RADIUS && dist < nearestDist) {
                nearestDist = dist;
                nearestEnemy = enemy.get();
            }
        }
    }
    
    if (nearestEnemy) {
        attackEntity(nearestEnemy);
    } else {
        wander();
    }
}

void AllyTank::defendBase() {
    isPatrolling = false;
    float distToBase = std::sqrt(
        std::pow(playerBasePosition.x - position.x, 2) + 
        std::pow(playerBasePosition.y - position.y, 2)
    );
    
    float idealDist = 120.0f;
    
    if (distToBase > idealDist + 30.0f) {
        sf::Vector2f dir = playerBasePosition - position;
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len > 0) {
            dir.x /= len;
            dir.y /= len;
            float newX = position.x + dir.x * speed * 0.8f * 0.016f;
            float newY = position.y + dir.y * speed * 0.8f * 0.016f;
            if (!checkBarrierCollision({newX, newY})) {
                position.x = newX;
                position.y = newY;
            }
        }
    } else if (distToBase < idealDist - 30.0f) {
        sf::Vector2f dir = position - playerBasePosition;
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len > 0) {
            dir.x /= len;
            dir.y /= len;
            float newX = position.x + dir.x * speed * 0.4f * 0.016f;
            float newY = position.y + dir.y * speed * 0.4f * 0.016f;
            if (!checkBarrierCollision({newX, newY})) {
                position.x = newX;
                position.y = newY;
            }
        }
    }
    
    position.x = std::max(20.0f, std::min(WorldConfig::WIDTH - 20.0f, position.x));
    position.y = std::max(20.0f, std::min(WorldConfig::HEIGHT - 20.0f, position.y));
    
    turretRotation = std::atan2(playerBasePosition.y - position.y, 
                                playerBasePosition.x - position.x) * 180.0f / 3.14159f;
}

void AllyTank::wander() {
    isPatrolling = true;
    
    float distToPatrol = std::sqrt(
        std::pow(patrolTarget.x - position.x, 2) + 
        std::pow(patrolTarget.y - position.y, 2)
    );
    
    if (distToPatrol < 30.0f || std::rand() % 100 == 0) {
        patrolTarget.x = 100.0f + static_cast<float>(std::rand() % 1800);
        patrolTarget.y = 100.0f + static_cast<float>(std::rand() % 1300);
    }
    
    sf::Vector2f dir = patrolTarget - position;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len > 5.0f) {
        dir.x /= len;
        dir.y /= len;
        sf::Vector2f steering = getSteeringFromObstacles(dir);
        float newX = position.x + (dir.x + steering.x) * speed * 0.8f * 0.016f;
        float newY = position.y + (dir.y + steering.y) * speed * 0.8f * 0.016f;
        if (!checkBarrierCollision({newX, newY})) {
            position.x = newX;
            position.y = newY;
        }
    }
    
    position.x = std::max(20.0f, std::min(WorldConfig::WIDTH - 20.0f, position.x));
    position.y = std::max(20.0f, std::min(WorldConfig::HEIGHT - 20.0f, position.y));
    
    turretRotation += 30.0f * 0.016f;
}

void AllyTank::attackEntity(Entity* target) {
    if (!target || !target->isAlive()) {
        clearTarget();
        return;
    }
    
    isPatrolling = false;
    
    sf::Vector2f dir = target->getPosition() - position;
    float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    
    if (dist > 0) {
        dir.x /= dist;
        dir.y /= dist;
    }
    
    turretRotation = std::atan2(dir.y, dir.x) * 180.0f / 3.14159f;
    
    float preferredDist = 180.0f;
    
    if (dist > preferredDist + 20.0f) {
        sf::Vector2f steering = getSteeringFromObstacles(dir);
        float newX = position.x + (dir.x + steering.x) * speed * 0.016f;
        float newY = position.y + (dir.y + steering.y) * speed * 0.016f;
        if (!checkBarrierCollision({newX, newY})) {
            position.x = newX;
            position.y = newY;
        }
        rotation = turretRotation;
    } else if (dist < preferredDist - 20.0f) {
        sf::Vector2f retreatDir = {-dir.x, -dir.y};
        sf::Vector2f steering = getSteeringFromObstacles(retreatDir);
        float newX = position.x + (retreatDir.x + steering.x) * speed * 0.5f * 0.016f;
        float newY = position.y + (retreatDir.y + steering.y) * speed * 0.5f * 0.016f;
        if (!checkBarrierCollision({newX, newY})) {
            position.x = newX;
            position.y = newY;
        }
        rotation = turretRotation;
    }
    
    position.x = std::max(20.0f, std::min(WorldConfig::WIDTH - 20.0f, position.x));
    position.y = std::max(20.0f, std::min(WorldConfig::HEIGHT - 20.0f, position.y));
    
    if (canShoot() && dist < 400.0f) {
        shoot();
    }
}

void AllyTank::render(sf::RenderWindow& window) {
    renderBody(window);
    renderTurret(window);
    
    float barWidth = 35.0f;
    float barHeight = 4.0f;
    float offsetY = 28.0f;
    
    sf::RectangleShape bg({barWidth, barHeight});
    bg.setOrigin({barWidth / 2, barHeight / 2});
    bg.setPosition({position.x, position.y - offsetY});
    bg.setFillColor(sf::Color(50, 50, 50));
    window.draw(bg);
    
    float healthRatio = static_cast<float>(health) / maxHealth;
    sf::RectangleShape healthBar({barWidth * healthRatio, barHeight});
    healthBar.setOrigin({barWidth * healthRatio / 2, barHeight / 2});
    healthBar.setPosition({position.x, position.y - offsetY});
    healthBar.setFillColor(sf::Color(0, 200, 0));
    window.draw(healthBar);
    
    if (hasTarget()) {
        sf::CircleShape selection(25.0f);
        selection.setOrigin({25.0f, 25.0f});
        selection.setPosition(position);
        selection.setFillColor(sf::Color::Transparent);
        selection.setOutlineColor(sf::Color(100, 255, 100));
        selection.setOutlineThickness(2);
        window.draw(selection);
    }
}

void AllyTank::setAttackTarget(Entity* target) {
    attackTarget = target;
}

void AllyTank::clearTarget() {
    attackTarget = nullptr;
}

void AllyTank::followPlayer() {
    attackTarget = nullptr;
}

void AllyTank::setAutoTargetEnabled(bool enabled) {
    autoTargetEnabled = enabled;
}

void AllyTank::setEnemyList(const std::vector<std::unique_ptr<EnemyTank>>* list) {
    enemyList = list;
}

void AllyTank::renderBody(sf::RenderWindow& window) {
    sf::RectangleShape body(sf::Vector2f(size.x, size.y));
    body.setOrigin({size.x / 2, size.y / 2});
    body.setPosition(position);
    body.setRotation(sf::degrees(rotation));
    body.setFillColor(sf::Color(61, 107, 30));
    body.setOutlineColor(sf::Color::Black);
    body.setOutlineThickness(2);
    window.draw(body);
}

void AllyTank::renderTurret(sf::RenderWindow& window) {
    sf::RectangleShape turret(sf::Vector2f(20.0f, 6.0f));
    turret.setOrigin({10.0f, 3.0f});
    turret.setPosition(position);
    turret.setRotation(sf::degrees(turretRotation));
    turret.setFillColor(sf::Color(35, 35, 35));
    window.draw(turret);
    
    sf::CircleShape base(8.0f);
    base.setOrigin({8.0f, 8.0f});
    base.setPosition(position);
    base.setFillColor(sf::Color(45, 45, 45));
    window.draw(base);
}
