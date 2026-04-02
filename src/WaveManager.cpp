#include "WaveManager.h"
#include "EnemyTank.h"
#include "Fort.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

WaveManager::WaveManager() {
    waves = {
        {3, 0, 0},
        {2, 1, 0},
        {4, 2, 0},
        {3, 2, 2},
        {5, 3, 2},
        {6, 4, 3},
        {8, 5, 4},
    };
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

WaveConfig WaveManager::getCurrentWave() const {
    if (currentWave < static_cast<int>(waves.size())) {
        return waves[currentWave];
    }
    return waves.back();
}

bool WaveManager::isWaveComplete() const {
    return remainingEnemies <= 0;
}

bool WaveManager::isGameComplete() const {
    return currentWave >= static_cast<int>(waves.size()) && remainingEnemies <= 0;
}

void WaveManager::startNextWave() {
    currentWave++;
    WaveConfig config = getCurrentWave();
    remainingEnemies = config.lightTanks + config.heavyTanks + config.forts;
}

std::vector<std::unique_ptr<EnemyTank>> WaveManager::spawnEnemies(const std::vector<sf::FloatRect>* barriers) {
    std::vector<std::unique_ptr<EnemyTank>> spawned;
    WaveConfig config = getCurrentZone();
    
    std::vector<EnemyTank*> heavyTanks;
    
    for (int i = 0; i < config.heavyTanks; i++) {
        auto pos = getSpawnPosition(false, barriers);
        auto heavy = std::make_unique<EnemyTank>(pos.x, pos.y, EntityType::EnemyHeavy);
        heavyTanks.push_back(heavy.get());
        spawned.push_back(std::move(heavy));
        
        for (int j = 0; j < 2; j++) {
            float angle = static_cast<float>(j) * 3.14159f;
            float escortX = pos.x + std::cos(angle) * 80.0f;
            float escortY = pos.y + std::sin(angle) * 80.0f;
            escortX = std::max(50.0f, std::min(1950.0f, escortX));
            escortY = std::max(50.0f, std::min(1450.0f, escortY));
            
            auto escort = std::make_unique<EnemyTank>(escortX, escortY, EntityType::EnemyLight);
            escort->setLeader(heavyTanks.back());
            spawned.push_back(std::move(escort));
        }
    }
    
    for (int i = 0; i < config.lightTanks; i++) {
        auto pos = getSpawnPosition(false, barriers);
        spawned.push_back(std::make_unique<EnemyTank>(pos.x, pos.y, EntityType::EnemyLight));
    }
    
    return spawned;
}

std::vector<std::unique_ptr<Fort>> WaveManager::spawnForts(const std::vector<sf::FloatRect>* barriers) {
    std::vector<std::unique_ptr<Fort>> spawned;
    WaveConfig config = getCurrentZone();
    
    for (int i = 0; i < config.forts; i++) {
        auto pos = getSpawnPosition(true, barriers);
        spawned.push_back(std::make_unique<Fort>(pos.x, pos.y));
    }
    
    return spawned;
}

WaveConfig WaveManager::getCurrentZone() const {
    if (currentWave < 1 || currentWave > static_cast<int>(waves.size())) {
        return {0, 0, 0};
    }
    return waves[currentWave - 1];
}

sf::Vector2f WaveManager::getSpawnPosition(bool isFort, const std::vector<sf::FloatRect>* barriers) {
    sf::Vector2u screenSize = sf::VideoMode::getDesktopMode().size;
    
    float margin = 50.0f;
    float extendedWidth = static_cast<float>(screenSize.x) + 500.0f;
    float minX = margin;
    float maxX = extendedWidth - margin;
    float minY = margin;
    float maxY = static_cast<float>(screenSize.y) - margin;
    
    sf::Vector2f pos;
    bool valid = false;
    int attempts = 0;
    
    while (!valid && attempts < 100) {
        attempts++;
        float x = minX + static_cast<float>(std::rand() % static_cast<int>(maxX - minX));
        float y = minY + static_cast<float>(std::rand() % static_cast<int>(maxY - minY));
        
        float centerX = screenSize.x / 2.0f;
        float centerY = screenSize.y / 2.0f;
        float distFromCenter = std::sqrt(std::pow(x - centerX, 2) + std::pow(y - centerY, 2));
        if (distFromCenter < 250.0f) {
            x = centerX + (x > centerX ? 250.0f : -250.0f);
        }
        
        x = std::max(minX, std::min(maxX, x));
        y = std::max(minY, std::min(maxY, y));
        
        valid = true;
        if (barriers) {
            sf::FloatRect spawnArea({x - 40, y - 40}, {80, 80});
            for (const auto& barrier : *barriers) {
                if (spawnArea.findIntersection(barrier).has_value()) {
                    valid = false;
                    break;
                }
            }
        }
        
        if (valid) {
            pos = {x, y};
        }
    }
    
    if (!valid) {
        pos = {screenSize.x / 2.0f + 300.0f, screenSize.y / 2.0f};
        pos.x = std::max(minX, std::min(maxX, pos.x));
        pos.y = std::max(minY, std::min(maxY, pos.y));
    }
    
    return pos;
}