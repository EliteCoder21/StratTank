#include "Tank.h"
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
    
    sf::Vector2u screenSize = sf::VideoMode::getDesktopMode().size;
    float margin = 20.0f;
    position.x = std::max(margin, std::min(static_cast<float>(screenSize.x) - margin, position.x));
    position.y = std::max(margin, std::min(static_cast<float>(screenSize.y) - margin, position.y));
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
    
    position.x = std::max(20.0f, std::min(WORLD_WIDTH - 20.0f, position.x));
    position.y = std::max(20.0f, std::min(WORLD_HEIGHT - 20.0f, position.y));
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

sf::Vector2f Tank::getSteeringFromObstacles(const sf::Vector2f& direction) {
    if (!barriers) return {0, 0};
    
    float lookAhead = 150.0f;
    float sideDist = 40.0f;
    float steerStrength = 2.0f;
    
    sf::Vector2f perpDir = {direction.y, -direction.x};
    
    sf::Vector2f ahead1 = {
        position.x + direction.x * lookAhead,
        position.y + direction.y * lookAhead
    };
    sf::Vector2f ahead2 = {
        position.x + direction.x * lookAhead * 0.5f + perpDir.x * sideDist,
        position.y + direction.y * lookAhead * 0.5f + perpDir.y * sideDist
    };
    sf::Vector2f ahead3 = {
        position.x + direction.x * lookAhead * 0.5f - perpDir.x * sideDist,
        position.y + direction.y * lookAhead * 0.5f - perpDir.y * sideDist
    };
    
    bool a1 = false, a2 = false, a3 = false;
    
    for (const auto& barrier : *barriers) {
        if (barrier.contains(ahead1)) a1 = true;
        if (barrier.contains(ahead2)) a2 = true;
        if (barrier.contains(ahead3)) a3 = true;
    }
    
    sf::Vector2f steering = {0, 0};
    
    if (a1) {
        if (a2 && !a3) {
            steering = {perpDir.x, perpDir.y};
        } else if (a3 && !a2) {
            steering = {-perpDir.x, -perpDir.y};
        } else if (a2 && a3) {
            steering = {-direction.x, -direction.y};
        } else {
            steering = (perpDir.x > perpDir.y) ? sf::Vector2f{perpDir.x, perpDir.y} : sf::Vector2f{-perpDir.x, -perpDir.y};
        }
    } else if (a2) {
        steering = {perpDir.x, perpDir.y};
    } else if (a3) {
        steering = {-perpDir.x, -perpDir.y};
    }
    
    float len = std::sqrt(steering.x * steering.x + steering.y * steering.y);
    if (len > 0) {
        steering.x /= len;
        steering.y /= len;
    }
    
    return {steering.x * steerStrength, steering.y * steerStrength};
}

bool Tank::isInsideBarrier() {
    return checkBarrierCollision(position);
}

void Tank::teleportToRandomPosition() {
    if (!barriers) return;
    
    float margin = 50.0f;
    sf::Vector2u screenSize = sf::VideoMode::getDesktopMode().size;
    
    for (int attempts = 0; attempts < 50; attempts++) {
        float x = margin + static_cast<float>(std::rand() % static_cast<int>(screenSize.x - margin * 2));
        float y = margin + static_cast<float>(std::rand() % static_cast<int>(screenSize.y - margin * 2));
        
        if (!checkBarrierCollision({x, y})) {
            position.x = x;
            position.y = y;
            return;
        }
    }
    
    position.x = screenSize.x / 2.0f;
    position.y = screenSize.y / 2.0f;
}