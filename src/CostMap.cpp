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
    const float SAFE_DIST = 5.0f;
    const float FULL_COST_DIST = 80.0f;
    
    for (int y = 0; y < gridHeight; y++) {
        for (int x = 0; x < gridWidth; x++) {
            float worldX = indexToWorld(x);
            float worldY = indexToWorld(y);
            
            float minDistToBarrier = 1e6f;
            
            for (const auto& barrier : barriers) {
                float closestX = std::max(barrier.position.x, std::min(worldX, barrier.position.x + barrier.size.x));
                float closestY = std::max(barrier.position.y, std::min(worldY, barrier.position.y + barrier.size.y));
                
                float dx = worldX - closestX;
                float dy = worldY - closestY;
                float dist = std::sqrt(dx * dx + dy * dy);
                
                minDistToBarrier = std::min(minDistToBarrier, dist);
            }
            
            float cost;
            if (minDistToBarrier < SAFE_DIST) {
                cost = MAX_COST_LOCAL;
            } else if (minDistToBarrier < FULL_COST_DIST) {
                float t = (minDistToBarrier - SAFE_DIST) / (FULL_COST_DIST - SAFE_DIST);
                float smoothT = t * t * (3.0f - 2.0f * t);
                cost = MAX_COST_LOCAL * (1.0f - smoothT) + SAFE_COST * smoothT;
            } else {
                cost = SAFE_COST;
            }
            
            costGrid[y * gridWidth + x] = cost;
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
