#include "Fort.h"
#include "Entity.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

Fort::Fort(float x, float y)
    : Entity(x, y, EntityType::Fort) {
    health = 200;
    maxHealth = 200;
    size = {60, 60};
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

void Fort::update(float deltaTime) {
    updateAI();
    updateCooldown(deltaTime);
    
    regenCooldown -= deltaTime;
    if (regenCooldown <= 0.0f && health < maxHealth) {
        heal(REGEN_AMOUNT);
        regenCooldown = REGEN_INTERVAL;
    }
    
    bombCooldown -= deltaTime;
    
    bombTargets.clear();
    bombTargetPosition = position;
    float totalX = 0, totalY = 0;
    int count = 0;
    
    for (const auto& ally : allyTargets) {
        if (ally && ally->isAlive()) {
            bombTargets.push_back(ally);
            totalX += ally->getPosition().x;
            totalY += ally->getPosition().y;
            count++;
        }
    }
    
    if (count > 0) {
        bombTargetPosition = {totalX / count, totalY / count};
        
        if (bombCooldown < 3.0f && bombCooldown > 0.0f) {
            bombWarningActive = true;
        } else {
            bombWarningActive = false;
        }
        
        if (bombCooldown <= 0.0f && bombCallback) {
            triggerBomb();
        }
    } else {
        bombWarningActive = false;
    }
}

void Fort::render(sf::RenderWindow& window) {
    sf::RectangleShape base(sf::Vector2f(size.x, size.y));
    base.setOrigin({size.x / 2, size.y / 2});
    base.setPosition(position);
    base.setFillColor(sf::Color(74, 74, 74));
    base.setOutlineColor(sf::Color(42, 42, 42));
    base.setOutlineThickness(3);
    window.draw(base);
    
    sf::RectangleShape inner(sf::Vector2f(size.x - 20, size.y - 20));
    inner.setOrigin({(size.x - 20) / 2, (size.y - 20) / 2});
    inner.setPosition(position);
    inner.setFillColor(sf::Color(100, 100, 100));
    window.draw(inner);
    
    sf::CircleShape core(size.x / 4);
    core.setOrigin({size.x / 4, size.x / 4});
    core.setPosition(position);
    core.setFillColor(sf::Color(60, 60, 60));
    window.draw(core);
    
    float turretLen = 40.0f + size.x / 3;
    sf::RectangleShape turret(sf::Vector2f(turretLen, 12.0f));
    turret.setOrigin({turretLen / 2, 6.0f});
    turret.setPosition(position);
    turret.setRotation(sf::degrees(turretRotation));
    turret.setFillColor(sf::Color(45, 45, 45));
    window.draw(turret);
    
    sf::CircleShape cannonBase(size.x / 5);
    cannonBase.setOrigin({size.x / 5, size.x / 5});
    cannonBase.setPosition(position);
    cannonBase.setFillColor(sf::Color(50, 50, 50));
    window.draw(cannonBase);
    
    if (showHealthBar) {
        float barWidth = 80.0f;
        float barHeight = 6.0f;
        float offsetY = 55.0f;
        
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
    
    if (bombWarningActive) {
        float pulse = 0.5f + 0.5f * std::sin(bombCooldown * 10.0f);
        sf::CircleShape warning(bombRadius * pulse);
        warning.setOrigin({bombRadius * pulse, bombRadius * pulse});
        warning.setPosition(bombTargetPosition);
        warning.setFillColor(sf::Color(255, 0, 0, 100));
        warning.setOutlineColor(sf::Color(255, 100, 0, 200));
        warning.setOutlineThickness(3);
        window.draw(warning);
        
        sf::CircleShape center(10.0f * pulse);
        center.setOrigin({10.0f * pulse, 10.0f * pulse});
        center.setPosition(bombTargetPosition);
        center.setFillColor(sf::Color(255, 200, 0));
        window.draw(center);
    }
}

void Fort::setPlayerPosition(sf::Vector2f pos) {
    playerPosition = pos;
}

void Fort::setAllyTargets(const std::vector<Entity*>& allies) {
    allyTargets.clear();
    for (const auto& ally : allies) {
        allyTargets.push_back(ally);
    }
}

void Fort::shoot() {
    if (canShoot() && projectileCallback) {
        fireCooldown = 1.0f / fireRate;
        float rad = turretRotation * 3.14159f / 180.0f;
        float spawnX = position.x + std::cos(rad) * 50.0f;
        float spawnY = position.y + std::sin(rad) * 50.0f;
        projectileCallback(spawnX, spawnY, turretRotation, damage, false);
    }
}

bool Fort::canShoot() const {
    return fireCooldown <= 0.0f;
}

void Fort::updateCooldown(float deltaTime) {
    if (fireCooldown > 0.0f) {
        fireCooldown -= deltaTime;
    }
}

void Fort::updateAI() {
    sf::Vector2f targetPos = {0, 0};
    float minDist = std::numeric_limits<float>::max();
    bool foundTarget = false;
    
    for (const auto& ally : allyTargets) {
        if (ally && ally->isAlive()) {
            float d = std::sqrt(std::pow(ally->getPosition().x - position.x, 2) + 
                              std::pow(ally->getPosition().y - position.y, 2));
            if (d < minDist) {
                minDist = d;
                targetPos = ally->getPosition();
                foundTarget = true;
            }
        }
    }
    
    if (!foundTarget) return;
    
    float targetAngle = std::atan2(targetPos.y - position.y, 
                                   targetPos.x - position.x) * 180.0f / 3.14159f;
    
    float angleDiff = targetAngle - turretRotation;
    while (angleDiff > 180) angleDiff -= 360;
    while (angleDiff < -180) angleDiff += 360;
    
    if (std::abs(angleDiff) > 1.0f) {
        turretRotation += (angleDiff > 0 ? 1.0f : -1.0f) * rotationSpeed;
    }
    
    if (std::abs(angleDiff) < 30.0f && canShoot()) {
        shoot();
    }
}

void Fort::triggerBomb() {
    if (bombCallback) {
        bombCallback(bombTargetPosition.x, bombTargetPosition.y, bombRadius);
    }
    bombCooldown = 10.0f + static_cast<float>(std::rand() % 5);
    bombWarningActive = false;
}