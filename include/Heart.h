#pragma once

#include "Entity.h"

class Heart : public Entity {
public:
    Heart(float x, float y, int healAmount = 25);
    
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    
    int getHealAmount() const { return healAmount; }
    bool isHeart() const override { return true; }

private:
    int healAmount;
    float bobTimer = 0.0f;
    float lifetime = 15.0f;
};
