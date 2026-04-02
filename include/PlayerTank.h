#pragma once

#include "Tank.h"

class PlayerTank : public Tank {
public:
    PlayerTank(float x, float y);

    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

    void handleInput(const sf::Keyboard::Key& key, bool pressed);
    void setAimDirection(sf::Vector2f direction);
    void aimAtMouse(sf::Vector2f mousePos);

    bool isMoving() const { return moving; }

private:
    bool moving = false;
    sf::Vector2f moveDirection = {0, 0};
    sf::Vector2f aimDirection = {1, 0};
};