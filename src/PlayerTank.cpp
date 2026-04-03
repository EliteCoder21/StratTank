#include "PlayerTank.h"
#include "WorldConfig.h"
#include <cmath>

PlayerTank::PlayerTank(float x, float y)
    : Tank(x, y, EntityType::Player, 100, 150.0f, 15, 3.0f) {
    size = {45, 45};
}

void PlayerTank::update(float deltaTime) {
    updateCooldown(deltaTime);
    
    float moveX = 0.0f;
    float moveY = 0.0f;
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) moveY -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) moveY += 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) moveX -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) moveX += 1.0f;
    
    if (moveX != 0.0f || moveY != 0.0f) {
        float len = std::sqrt(moveX * moveX + moveY * moveY);
        moveX /= len;
        moveY /= len;
        
        sf::Vector2f steering = getSteeringFromObstacles({moveX, moveY});
        float newX = position.x + (moveX + steering.x) * speed * deltaTime;
        float newY = position.y + (moveY + steering.y) * speed * deltaTime;
        
        if (!checkBarrierCollision({newX, newY})) {
            position.x = newX;
            position.y = newY;
        }
        
        rotation = std::atan2(moveY, moveX) * 180.0f / 3.14159f;
        
        position.x = std::max(22.0f, std::min(WorldConfig::WIDTH - 22.0f, position.x));
        position.y = std::max(22.0f, std::min(WorldConfig::HEIGHT - 22.0f, position.y));
    }
}

void PlayerTank::render(sf::RenderWindow& window) {
    renderBody(window);
    renderTurret(window);
    
    float barWidth = 40.0f;
    float barHeight = 5.0f;
    float offsetY = 35.0f;
    
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
}

void PlayerTank::handleInput(const sf::Keyboard::Key& key, bool pressed) {
}

void PlayerTank::setAimDirection(sf::Vector2f dir) {
    aimDirection = dir;
    float angle = std::atan2(dir.y, dir.x) * 180.0f / 3.14159f;
    turretRotation = angle;
}

void PlayerTank::aimAtMouse(sf::Vector2f mousePos) {
    sf::Vector2f dir = mousePos - position;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len > 5.0f) {
        dir.x /= len;
        dir.y /= len;
        setAimDirection(dir);
    } else if (len == 0 || std::isnan(len)) {
        setAimDirection({1, 0});
    }
}