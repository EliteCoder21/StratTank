#include "Heart.h"
#include <cmath>

Heart::Heart(float x, float y, int healAmt)
    : Entity(x, y, EntityType::Player), healAmount(healAmt) {
    size = {30, 30};
    health = 1;
}

void Heart::update(float deltaTime) {
    bobTimer += deltaTime * 3.0f;
    lifetime -= deltaTime;
    
    if (lifetime <= 0) {
        markForDeletion();
    }
}

void Heart::render(sf::RenderWindow& window) {
    float bobOffset = std::sin(bobTimer) * 5.0f;
    float pulse = 1.0f + std::sin(bobTimer * 2.0f) * 0.1f;
    
    sf::CircleShape heart(15.0f * pulse);
    heart.setOrigin({15.0f * pulse, 15.0f * pulse});
    heart.setPosition({position.x, position.y + bobOffset});
    heart.setFillColor(sf::Color(255, 50, 50));
    window.draw(heart);
    
    sf::CircleShape highlight(5.0f);
    highlight.setOrigin({3.0f, 3.0f});
    highlight.setPosition({position.x - 5.0f, position.y - 5.0f + bobOffset});
    highlight.setFillColor(sf::Color(255, 150, 150));
    window.draw(highlight);
}
