#include "EnemyTank.h"
#include "Entity.h"
#include "WorldConfig.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

EnemyTank::EnemyTank(float x, float y, EntityType type)
    : Tank(x, y, type, 
           type == EntityType::EnemyLight ? 30 : 80,
           type == EntityType::EnemyLight ? 120.0f : 60.0f,
           type == EntityType::EnemyLight ? 10 : 20,
           type == EntityType::EnemyLight ? 2.5f : 1.5f) {
    size = type == EntityType::EnemyLight ? sf::Vector2f(30, 30) : sf::Vector2f(45, 45);
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    patrolTarget = position;
}

void EnemyTank::update(float deltaTime) {
    Tank::update(deltaTime);
    updateAI(deltaTime);
    updateCooldown(deltaTime);
    
    healCooldown -= deltaTime;
    if (healCooldown <= 0.0f && lastCombatTime > 3.0f && health < maxHealth) {
        heal(1);
        healCooldown = 1.0f;
    }
    if (lastCombatTime > 0.0f) {
        lastCombatTime -= deltaTime;
    }
}

void EnemyTank::takeDamage(int damage) {
    Entity::takeDamage(damage);
    lastCombatTime = 5.0f;
    healCooldown = 1.0f;
}

void EnemyTank::render(sf::RenderWindow& window) {
    renderBody(window);
    renderTurret(window);
}

void EnemyTank::setPlayerPosition(sf::Vector2f pos) {
    playerPosition = pos;
}

void EnemyTank::setAllyTargets(const std::vector<Entity*>& allies) {
    allyTargets.clear();
    for (const auto& ally : allies) {
        allyTargets.push_back(ally);
    }
}

void EnemyTank::updateAI(float deltaTime) {
    aiUpdateTimer += deltaTime;
    if (aiUpdateTimer < aiUpdateInterval) return;
    aiUpdateTimer = 0.0f;

    const float DETECTION_RADIUS = 800.0f;
    const float ATTACK_RADIUS = 500.0f;
    const float FLEE_HEALTH_THRESHOLD = 0.35f;
    const float LEADER_DISTANCE = 60.0f;
    
    float healthRatio = static_cast<float>(health) / maxHealth;
    isFleeing = healthRatio < FLEE_HEALTH_THRESHOLD;
    
    sf::Vector2f targetPos = patrolTarget;
    float minDist = std::numeric_limits<float>::max();
    bool foundTarget = false;
    bool targetIsEnemy = false;

    if (hasLeader() && leaderTank && leaderTank->isAlive()) {
        float leaderDist = std::sqrt(std::pow(leaderTank->getPosition().x - position.x, 2) + 
                                    std::pow(leaderTank->getPosition().y - position.y, 2));
        
        if (leaderDist > LEADER_DISTANCE * 1.5f) {
            targetPos = leaderTank->getPosition();
            minDist = leaderDist;
            foundTarget = true;
            targetIsEnemy = true;
        }
    }

    if (!foundTarget) {
        float playerDist = std::sqrt(std::pow(playerPosition.x - position.x, 2) + 
                                    std::pow(playerPosition.y - position.y, 2));
        float baseDist = std::sqrt(std::pow(playerBasePosition.x - position.x, 2) + 
                                   std::pow(playerBasePosition.y - position.y, 2));
        
        if (playerDist < DETECTION_RADIUS) {
            targetPos = playerPosition;
            minDist = playerDist;
            foundTarget = true;
            targetIsEnemy = false;
        }
        
        if (baseDist < DETECTION_RADIUS && baseDist < minDist) {
            targetPos = playerBasePosition;
            minDist = baseDist;
            foundTarget = true;
            targetIsEnemy = false;
        }

        for (const auto& ally : allyTargets) {
            if (!ally || !ally->isAlive()) continue;
            
            EntityType targetType = ally->getType();
            if (targetType == EntityType::EnemyLight) continue;
            if (targetType == EntityType::EnemyHeavy) continue;
            if (targetType == EntityType::Fort) continue;
            
            float d = std::sqrt(std::pow(ally->getPosition().x - position.x, 2) + 
                              std::pow(ally->getPosition().y - position.y, 2));
            if (d < DETECTION_RADIUS && d < minDist) {
                minDist = d;
                targetPos = ally->getPosition();
                foundTarget = true;
                targetIsEnemy = false;
            }
        }
    }

    if (!foundTarget) {
        isPatrolling = true;
        updatePatrol(deltaTime);
        targetPos = patrolTarget;
    } else {
        isPatrolling = false;
    }

    float dist = std::sqrt(std::pow(targetPos.x - position.x, 2) + 
                          std::pow(targetPos.y - position.y, 2));
    
    if (dist > 0 && !targetIsEnemy) {
        sf::Vector2f dir = { (targetPos.x - position.x) / dist, 
                           (targetPos.y - position.y) / dist };
        
        if (isFleeing && foundTarget) {
            turretRotation = std::atan2(-dir.y, -dir.x) * 180.0f / 3.14159f;
            sf::Vector2f fleeDir = {-dir.x, -dir.y};
            float newX = position.x + fleeDir.x * speed * deltaTime * 1.2f;
            float newY = position.y + fleeDir.y * speed * deltaTime * 1.2f;
            if (!checkBarrierCollision({newX, newY})) {
                position.x = newX;
                position.y = newY;
            }
            rotation = turretRotation;
        } else {
            turretRotation = std::atan2(dir.y, dir.x) * 180.0f / 3.14159f;

            if (type == EntityType::EnemyLight && !isPatrolling && !isFleeing) {
                strafeTimer += deltaTime;
                if (strafeTimer > 2.0f) {
                    strafeTimer = 0.0f;
                    isFlanking = !isFlanking;
                    float angle = static_cast<float>(std::rand() % 360) * 3.14159f / 180.0f;
                    flankDirection = {std::cos(angle), std::sin(angle)};
                }
                
                if (isFlanking) {
                    sf::Vector2f strafeDir = dir;
                    if (std::abs(dir.x) > 0.1f || std::abs(dir.y) > 0.1f) {
                        float perpX = -dir.y;
                        float perpY = dir.x;
                        strafeDir = {dir.x + perpX * 0.5f, dir.y + perpY * 0.5f};
                    }
                    float len = std::sqrt(strafeDir.x * strafeDir.x + strafeDir.y * strafeDir.y);
                    if (len > 0) {
                        strafeDir.x /= len;
                        strafeDir.y /= len;
                    }
                    float newX = position.x + strafeDir.x * speed * deltaTime * 0.8f;
                    float newY = position.y + strafeDir.y * speed * deltaTime * 0.8f;
                    if (!checkBarrierCollision({newX, newY})) {
                        position.x = newX;
                        position.y = newY;
                    }
                }
            }

            if (isPatrolling) {
                if (dist > 10.0f) {
                    float newX = position.x + dir.x * speed * 0.8f * deltaTime;
                    float newY = position.y + dir.y * speed * 0.8f * deltaTime;
                    if (!checkBarrierCollision({newX, newY})) {
                        position.x = newX;
                        position.y = newY;
                    }
                }
            } else if (dist > ATTACK_RADIUS) {
                float newX = position.x + dir.x * speed * deltaTime;
                float newY = position.y + dir.y * speed * deltaTime;
                if (!checkBarrierCollision({newX, newY})) {
                    position.x = newX;
                    position.y = newY;
                }
            } else if (dist < ATTACK_RADIUS * 0.5f) {
                float newX = position.x + (-dir.x) * speed * 0.6f * deltaTime;
                float newY = position.y + (-dir.y) * speed * 0.6f * deltaTime;
                if (!checkBarrierCollision({newX, newY})) {
                    position.x = newX;
                    position.y = newY;
                }
            }

            rotation = turretRotation;

            if (canShoot() && dist < ATTACK_RADIUS && !isPatrolling && !isFleeing && hasLineOfSight(targetPos)) {
                shoot();
            }
        }
    }

    position.x = std::max(20.0f, std::min(WorldConfig::WIDTH - 20.0f, position.x));
    position.y = std::max(20.0f, std::min(WorldConfig::HEIGHT - 20.0f, position.y));
}

void EnemyTank::updatePatrol(float deltaTime) {
    const float PATROL_SPEED = 4.0f;
    const float TURN_RATE = 4.0f;
    const float DIRECTION_CHANGE_TIME = 2.0f;
    
    patrolTimer += deltaTime;
    
    if (patrolTimer > DIRECTION_CHANGE_TIME || patrolState == 0) {
        patrolTimer = 0.0f;
        patrolState = (patrolState + 1) % 4;
        
        switch (patrolState) {
            case 0:
                patrolDirection = {1.0f, 0.0f};
                break;
            case 1:
                patrolDirection = {0.0f, 1.0f};
                break;
            case 2:
                patrolDirection = {-1.0f, 0.0f};
                break;
            case 3:
                patrolDirection = {0.0f, -1.0f};
                break;
        }
        
        patrolTarget.x = position.x + patrolDirection.x * patrolRadius * 2.0f;
        patrolTarget.y = position.y + patrolDirection.y * patrolRadius * 2.0f;
        
        patrolTarget.x = std::max(50.0f, std::min(WorldConfig::WIDTH - 50.0f, patrolTarget.x));
        patrolTarget.y = std::max(50.0f, std::min(WorldConfig::HEIGHT - 50.0f, patrolTarget.y));
    }
    
    float currentAngle = turretRotation;
    float targetAngle = std::atan2(patrolDirection.y, patrolDirection.x) * 180.0f / 3.14159f;
    
    float angleDiff = targetAngle - currentAngle;
    while (angleDiff > 180) angleDiff -= 360;
    while (angleDiff < -180) angleDiff += 360;
    
    if (std::abs(angleDiff) > TURN_RATE) {
        turretRotation += (angleDiff > 0 ? TURN_RATE : -TURN_RATE);
    } else {
        turretRotation = targetAngle;
    }
    
    if (type == EntityType::EnemyLight) {
        float strafeDir = (std::rand() % 2 == 0) ? 1.0f : -1.0f;
        sf::Vector2f perpDir = {-patrolDirection.y * strafeDir, patrolDirection.x * strafeDir};
        float newX = position.x + patrolDirection.x * speed * PATROL_SPEED * deltaTime + perpDir.x * speed * 0.8f * deltaTime;
        float newY = position.y + patrolDirection.y * speed * PATROL_SPEED * deltaTime + perpDir.y * speed * 0.8f * deltaTime;
        if (!checkBarrierCollision({newX, newY})) {
            position.x = newX;
            position.y = newY;
        } else {
            patrolTimer = DIRECTION_CHANGE_TIME;
        }
    } else {
        float newX = position.x + patrolDirection.x * speed * PATROL_SPEED * deltaTime;
        float newY = position.y + patrolDirection.y * speed * PATROL_SPEED * deltaTime;
        if (!checkBarrierCollision({newX, newY})) {
            position.x = newX;
            position.y = newY;
        } else {
            patrolTimer = DIRECTION_CHANGE_TIME;
        }
    }
    
    rotation = turretRotation;
}

void EnemyTank::renderBody(sf::RenderWindow& window) {
    sf::RectangleShape body(sf::Vector2f(size.x, size.y));
    body.setOrigin({size.x / 2, size.y / 2});
    body.setPosition(position);
    body.setRotation(sf::degrees(rotation));
    body.setFillColor(type == EntityType::EnemyLight ? 
                     sf::Color(139, 69, 19) : sf::Color(92, 42, 42));
    body.setOutlineColor(sf::Color::Black);
    body.setOutlineThickness(2);
    window.draw(body);
}

void EnemyTank::renderTurret(sf::RenderWindow& window) {
    float turretLen = type == EntityType::EnemyLight ? 18.0f : 25.0f;
    float turretWid = type == EntityType::EnemyLight ? 5.0f : 8.0f;
    
    sf::RectangleShape turret(sf::Vector2f(turretLen, turretWid));
    turret.setOrigin({turretLen / 2, turretWid / 2});
    turret.setPosition(position);
    turret.setRotation(sf::degrees(turretRotation));
    turret.setFillColor(sf::Color(30, 30, 30));
    window.draw(turret);
    
    float baseRadius = type == EntityType::EnemyLight ? 7.0f : 10.0f;
    sf::CircleShape base(baseRadius);
    base.setOrigin({baseRadius, baseRadius});
    base.setPosition(position);
    base.setFillColor(sf::Color(40, 40, 40));
    window.draw(base);
    
    float barWidth = type == EntityType::EnemyLight ? 30.0f : 45.0f;
    float barHeight = 4.0f;
    float offsetY = type == EntityType::EnemyLight ? 25.0f : 35.0f;
    
    sf::RectangleShape bg({barWidth, barHeight});
    bg.setOrigin({barWidth / 2, barHeight / 2});
    bg.setPosition({position.x, position.y - offsetY});
    bg.setFillColor(sf::Color(50, 50, 50));
    window.draw(bg);
    
    float healthRatio = static_cast<float>(health) / maxHealth;
    sf::RectangleShape healthBar({barWidth * healthRatio, barHeight});
    healthBar.setOrigin({barWidth * healthRatio / 2, barHeight / 2});
    healthBar.setPosition({position.x, position.y - offsetY});
    healthBar.setFillColor(sf::Color(200, 50, 50));
    window.draw(healthBar);
}
