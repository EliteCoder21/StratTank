#include "Tank.h"
#include "WorldConfig.h"
#include <cmath>

Tank::Tank(float x, float y, EntityType type, int maxHealthVal, float speedVal, int damageVal, float fireRateVal)
    : Entity(x, y, type), speed(speedVal), damage(damageVal), fireRate(fireRateVal) {
    maxHealth = maxHealthVal;
    health = maxHealth;
    size = {40, 40};
}

void Tank::update(float deltaTime) {
    updateCooldown(deltaTime);
    
    if (hasTarget) {
        sf::Vector2f dir = targetPosition - position;
        float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (dist > 10.0f) {
            dir.x /= dist;
            dir.y /= dist;
            
            turretRotation = std::atan2(dir.y, dir.x) * 180.0f / 3.14159f;
            
            float moveSpeed = speed * 2.0f;
            float newX = position.x + dir.x * moveSpeed * deltaTime;
            float newY = position.y + dir.y * moveSpeed * deltaTime;
            
            if (!checkBarrierCollision({newX, newY})) {
                position.x = newX;
                position.y = newY;
            }
            
            rotation = turretRotation;
        } else {
            hasTarget = false;
        }
    }
    
    bool outOfBounds = position.x < WorldConfig::MARGIN || position.x > WorldConfig::WIDTH - WorldConfig::MARGIN || 
                       position.y < WorldConfig::MARGIN || position.y > WorldConfig::HEIGHT - WorldConfig::MARGIN;
    if (outOfBounds) {
        teleportToRandomPosition();
    }
}

void Tank::render(sf::RenderWindow& window) {
    renderBody(window);
    renderTurret(window);
}

void Tank::renderBody(sf::RenderWindow& window) {
    sf::RectangleShape body(sf::Vector2f(size.x, size.y));
    body.setOrigin({size.x / 2, size.y / 2});
    body.setPosition(position);
    body.setRotation(sf::degrees(rotation));
    
    if (type == EntityType::Player) {
        body.setFillColor(sf::Color(100, 180, 50));
    } else if (type == EntityType::Ally) {
        body.setFillColor(sf::Color(61, 107, 30));
    } else if (type == EntityType::EnemyLight) {
        body.setFillColor(sf::Color(139, 69, 19));
    } else if (type == EntityType::EnemyHeavy) {
        body.setFillColor(sf::Color(92, 42, 42));
    }
    body.setOutlineColor(sf::Color::Black);
    body.setOutlineThickness(2);
    
    window.draw(body);
}

void Tank::renderTurret(sf::RenderWindow& window) {
    sf::RectangleShape turret(sf::Vector2f(25.0f, 8.0f));
    turret.setOrigin({12.5f, 4.0f});
    turret.setPosition(position);
    turret.setRotation(sf::degrees(turretRotation));
    turret.setFillColor(sf::Color(40, 40, 40));
    
    window.draw(turret);

    sf::CircleShape base(10.0f);
    base.setOrigin({10.0f, 10.0f});
    base.setPosition(position);
    base.setFillColor(sf::Color(50, 50, 50));
    window.draw(base);
}

void Tank::shoot() {
    if (canShoot() && projectileCallback) {
        fireCooldown = 1.0f / fireRate;
        float rad = turretRotation * 3.14159f / 180.0f;
        float spawnX = position.x + std::cos(rad) * 30.0f;
        float spawnY = position.y + std::sin(rad) * 30.0f;
        projectileCallback(spawnX, spawnY, turretRotation, damage, type == EntityType::Player || type == EntityType::Ally);
    }
}

bool Tank::canShoot() const {
    return fireCooldown <= 0.0f;
}

void Tank::updateCooldown(float deltaTime) {
    if (fireCooldown > 0.0f) {
        fireCooldown -= deltaTime;
    }
}

void Tank::move(float dx, float deltaTime) {
    float newRotation = rotation + dx * 90.0f * deltaTime;
    setRotation(newRotation);
    
    float rad = rotation * 3.14159f / 180.0f;
    float dxPos = std::cos(rad) * speed * deltaTime;
    float dyPos = std::sin(rad) * speed * deltaTime;
    
    float newX = position.x + dxPos;
    float newY = position.y + dyPos;
    
    if (!checkBarrierCollision({newX, newY})) {
        position.x = newX;
        position.y = newY;
    }
}

void Tank::setTargetPosition(sf::Vector2f target) {
    targetPosition = target;
    hasTarget = true;
}

bool Tank::checkBarrierCollision(const sf::Vector2f& newPos) {
    if (!barriers) return false;
    
    float halfSize = std::max(size.x, size.y) / 2.0f + 5.0f;
    sf::FloatRect tankBounds(
        {newPos.x - halfSize, newPos.y - halfSize},
        {halfSize * 2, halfSize * 2}
    );
    
    for (const auto& barrier : *barriers) {
        sf::FloatRect expandedBarrier = barrier;
        expandedBarrier.position.x -= 5.0f;
        expandedBarrier.position.y -= 5.0f;
        expandedBarrier.size.x += 10.0f;
        expandedBarrier.size.y += 10.0f;
        
        if (tankBounds.findIntersection(expandedBarrier).has_value()) {
            return true;
        }
    }
    return false;
}

bool Tank::isInsideBarrier() {
    return checkBarrierCollision(position);
}

bool Tank::hasLineOfSight(const sf::Vector2f& targetPos) const {
    if (!barriers) return true;
    
    sf::Vector2f dir = targetPos - position;
    float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (dist < 1.0f) return true;
    
    dir.x /= dist;
    dir.y /= dist;
    
    const int NUM_CHECKS = 10;
    for (int i = 1; i < NUM_CHECKS; i++) {
        float t = static_cast<float>(i) / NUM_CHECKS;
        sf::Vector2f checkPos = {
            position.x + dir.x * dist * t,
            position.y + dir.y * dist * t
        };
        
        for (const auto& barrier : *barriers) {
            if (barrier.contains(checkPos)) {
                return false;
            }
        }
    }
    
    return true;
}

void Tank::teleportToRandomPosition() {
    if (!barriers) return;
    
    float margin = 50.0f;
    
    for (int attempts = 0; attempts < 50; attempts++) {
        float x = margin + static_cast<float>(std::rand() % static_cast<int>(WorldConfig::WIDTH - margin * 2));
        float y = margin + static_cast<float>(std::rand() % static_cast<int>(WorldConfig::HEIGHT - margin * 2));
        
        if (!checkBarrierCollision({x, y})) {
            position.x = x;
            position.y = y;
            return;
        }
    }
    
    position.x = WorldConfig::WIDTH / 2.0f;
    position.y = WorldConfig::HEIGHT / 2.0f;
}