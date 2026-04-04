#include "CostMap.h"
#include <algorithm>
#include <cmath>

CostMap::CostMap()
    : gridWidth(0), gridHeight(0), cellSize(10.0f), worldWidth(0), worldHeight(0) {
}

void CostMap::generate(const std::vector<sf::FloatRect>& barriers, float wWidth, float wHeight) {
    worldWidth = wWidth;
    worldHeight = wHeight;
    
    gridWidth = static_cast<int>(std::ceil(worldWidth / cellSize));
    gridHeight = static_cast<int>(std::ceil(worldHeight / cellSize));
    
    costGrid.resize(gridWidth * gridHeight, SAFE_COST);
    
    const float MAX_COST_LOCAL = MAX_COST * 100.0f;
    const float SIGMA = 50.0f;
    
    for (int y = 0; y < gridHeight; y++) {
        for (int x = 0; x < gridWidth; x++) {
            float worldX = indexToWorld(x);
            float worldY = indexToWorld(y);
            
            float totalCost = SAFE_COST;
            
            for (const auto& barrier : barriers) {
                float centerX = barrier.position.x + barrier.size.x / 2.0f;
                float centerY = barrier.position.y + barrier.size.y / 2.0f;
                
                float dx = std::abs(worldX - centerX);
                float dy = std::abs(worldY - centerY);
                float dist = std::sqrt(dx * dx + dy * dy);
                
                float cost = MAX_COST_LOCAL * std::exp(-(dist * dist) / (2.0f * SIGMA * SIGMA));
                totalCost = std::max(totalCost, cost);
            }
            
            costGrid[y * gridWidth + x] = totalCost;
        }
    }
}

int CostMap::worldToIndex(float worldCoord) const {
    int idx = static_cast<int>(worldCoord / cellSize);
    return std::max(0, std::min(idx, gridWidth - 1));
}

float CostMap::indexToWorld(int index) const {
    return static_cast<float>(index) * cellSize;
}

float CostMap::getCost(float x, float y) const {
    if (!isValid()) return SAFE_COST;
    
    int ix = worldToIndex(x);
    int iy = worldToIndex(y);
    
    if (ix < 0 || ix >= gridWidth || iy < 0 || iy >= gridHeight) {
        return MAX_COST * 100.0f;
    }
    
    return costGrid[iy * gridWidth + ix];
}

sf::Vector2f CostMap::getGradient(float x, float y) const {
    if (!isValid()) return {0, 0};
    
    float eps = cellSize * 0.5f;
    
    float costLeft = getCost(x - eps, y);
    float costRight = getCost(x + eps, y);
    float costUp = getCost(x, y - eps);
    float costDown = getCost(x, y + eps);
    
    float gradX = (costRight - costLeft) / (2.0f * eps) * 10.0f;
    float gradY = (costDown - costUp) / (2.0f * eps) * 10.0f;
    
    return {gradX, gradY};
}
