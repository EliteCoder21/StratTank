#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <vector>
#include <cmath>

class CostMap {
public:
    CostMap();
    
    void generate(const std::vector<sf::FloatRect>& barriers, float worldWidth, float worldHeight);
    
    sf::Vector2f getGradient(float x, float y) const;
    float getCost(float x, float y) const;
    
    bool isValid() const { return gridWidth > 0 && gridHeight > 0; }
    
private:
    std::vector<float> costGrid;
    int gridWidth;
    int gridHeight;
    float cellSize;
    float worldWidth;
    float worldHeight;
    
    int worldToIndex(float worldCoord) const;
    float indexToWorld(int index) const;
    
    static constexpr float MAX_COST = 100.0f;
    static constexpr float SAFE_COST = 1.0f;
    static constexpr float COST_FALLOFF_DISTANCE = 50.0f;
};
