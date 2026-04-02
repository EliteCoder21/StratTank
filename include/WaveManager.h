#pragma once

#include <vector>
#include <memory>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "Entity.h"

class EnemyTank;
class Fort;

struct WaveConfig {
    int lightTanks;
    int heavyTanks;
    int forts;
};

class WaveManager {
public:
    WaveManager();

    WaveConfig getCurrentWave() const;
    int getWaveNumber() const { return currentWave; }
    bool isWaveComplete() const;
    bool isGameComplete() const;

    void startNextWave();
    std::vector<std::unique_ptr<EnemyTank>> spawnEnemies(const std::vector<sf::FloatRect>* barriers = nullptr);
    std::vector<std::unique_ptr<Fort>> spawnForts(const std::vector<sf::FloatRect>* barriers = nullptr);

    void enemyDestroyed() { remainingEnemies--; }
    int getRemainingEnemies() const { return remainingEnemies; }

private:
    WaveConfig getCurrentZone() const;
    sf::Vector2f getSpawnPosition(bool isFort, const std::vector<sf::FloatRect>* barriers);
    int currentWave = 0;
    int remainingEnemies = 0;
    std::vector<WaveConfig> waves;
};