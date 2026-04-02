#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <list>

#include "Entity.h"
#include "PlayerTank.h"
#include "AllyTank.h"
#include "EnemyTank.h"
#include "Fort.h"
#include "Projectile.h"
#include "Heart.h"
#include "ParticleSystem.h"
#include "WaveManager.h"
#include "HUD.h"

class Game {
public:
    Game();
    ~Game();

    void run();

private:
    void processEvents();
    void update(float deltaTime);
    void render();

    void spawnProjectile(std::unique_ptr<Projectile> projectile);
    void spawnHeart(float x, float y);
    void handleBombExplosion(float cx, float cy, float radius);
    void checkCollisions();
    void handlePlayerInput();
    void handleAllyCommands();
    void firePlayerWeapon();
    void handleAllyCommand(sf::Vector2f worldPos);
    void generateBarriers();
    void spawnEnemyBase();

    std::unique_ptr<PlayerTank> player;
    std::unique_ptr<Fort> playerBase;
    std::vector<std::unique_ptr<AllyTank>> allies;
    std::vector<std::unique_ptr<EnemyTank>> enemies;
    std::vector<std::unique_ptr<Fort>> forts;
    std::vector<sf::FloatRect> barriers;
    std::list<std::unique_ptr<Projectile>> projectiles;
    std::vector<std::unique_ptr<Heart>> hearts;
    
    ParticleSystem particles;
    WaveManager waveManager;
    HUD hud;
    float heartSpawnTimer = 0.0f;

    sf::RenderWindow window;
    sf::Clock clock;
    GameState state = GameState::Playing;

    int selectedAllyIndex = -1;
    int score = 0;
    bool shiftPressed = false;

    sf::Vector2f cameraPosition = {0, 0};
    const float WORLD_WIDTH = 2000.0f;
    const float WORLD_HEIGHT = 1500.0f;
    const float VIEW_WIDTH = 1280.0f;
    const float VIEW_HEIGHT = 720.0f;
};