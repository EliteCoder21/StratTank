#include "Game.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>

Game::Game() 
    : window(sf::VideoMode::getDesktopMode(), "StratTank - Strategy Tank Game", sf::State::Fullscreen) {
    
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    window.setFramerateLimit(60);
    
    playerBase = std::make_unique<Fort>(1000.0f, 650.0f);
    playerBase->setProjectileCallback([this](float x, float y, float angle, int dmg, bool isPlayer) {
        this->spawnProjectile(std::make_unique<Projectile>(x, y, angle, dmg, true));
    });
    playerBase->setSize({100, 100});
    playerBase->setMaxHealth(3000);
    playerBase->setHealth(3000);
    playerBase->setShowHealthBar(false);
    
    player = std::make_unique<PlayerTank>(1000.0f, 750.0f);
    
    for (int i = 0; i < 2; i++) {
        auto ally = std::make_unique<AllyTank>(900.0f + i * 200.0f, 750.0f, allies.size());
        ally->setGuardingBase(true);
        allies.push_back(std::move(ally));
    }
    
    for (int i = 0; i < 4; i++) {
        float angle = static_cast<float>(i) * 1.5708f;
        float x = 1000.0f + std::cos(angle) * 400.0f;
        float y = 750.0f + std::sin(angle) * 300.0f;
        auto ally = std::make_unique<AllyTank>(x, y, allies.size());
        ally->setGuardingBase(false);
        allies.push_back(std::move(ally));
    }
    
    generateBarriers();
    
    waveManager.startNextWave();
    auto newEnemies = waveManager.spawnEnemies(&barriers);
    for (auto& enemy : newEnemies) {
        enemies.push_back(std::move(enemy));
    }
    spawnEnemyBase();
}

void Game::generateBarriers() {
    barriers.clear();
    int numBarriers = 15 + std::rand() % 12;
    
    sf::Vector2u screenSize = window.getSize();
    
    for (int i = 0; i < numBarriers; i++) {
        float width = 40.0f + static_cast<float>(std::rand() % 120);
        float height = 40.0f + static_cast<float>(std::rand() % 120);
        
        float x, y;
        int attempts = 0;
        bool valid = false;
        
        while (!valid && attempts < 50) {
            x = 100.0f + static_cast<float>(std::rand() % static_cast<int>(screenSize.x - 200));
            y = 100.0f + static_cast<float>(std::rand() % static_cast<int>(screenSize.y - 200));
            
            sf::FloatRect newBarrier({x, y}, {width, height});
            valid = true;
            
            sf::FloatRect playerBaseArea(playerBase->getPosition() - sf::Vector2f(200, 200), {400, 400});
            if (newBarrier.findIntersection(playerBaseArea).has_value()) {
                valid = false;
            }
            
            for (const auto& barrier : barriers) {
                if (newBarrier.findIntersection(barrier).has_value()) {
                    valid = false;
                    break;
                }
            }
            
            attempts++;
        }
        
        if (valid) {
            barriers.push_back(sf::FloatRect({x, y}, {width, height}));
        }
    }
}

void Game::spawnEnemyBase() {
    sf::Vector2u screenSize = window.getSize();
    float centerX = screenSize.x / 2.0f;
    float centerY = screenSize.y / 2.0f;
    float extendedWidth = static_cast<float>(screenSize.x) + 500.0f;
    
    float angle = static_cast<float>(std::rand() % 360) * 3.14159f / 180.0f;
    float dist = std::min(screenSize.x, screenSize.y) * 0.4f + static_cast<float>(std::rand() % 200);
    float x = centerX + std::cos(angle) * dist;
    float y = centerY + std::sin(angle) * dist;
    
    float margin = 100.0f;
    x = std::max(margin, std::min(extendedWidth - margin, x));
    y = std::max(margin, std::min(static_cast<float>(screenSize.y) - margin, y));
    
    auto base = std::make_unique<Fort>(x, y);
    base->setProjectileCallback([this](float px, float py, float ang, int dmg, bool isPlayer) {
        this->spawnProjectile(std::make_unique<Projectile>(px, py, ang, dmg, isPlayer));
    });
    base->setBombCallback([this](float cx, float cy, float radius) {
        this->handleBombExplosion(cx, cy, radius);
    });
    base->setSize({80, 80});
    base->setMaxHealth(150);
    base->setHealth(150);
    forts.push_back(std::move(base));
    
    float guardX1 = std::max(50.0f, std::min(extendedWidth - 50.0f, x + 80.0f));
    float guardY1 = std::max(50.0f, std::min(static_cast<float>(screenSize.y) - 50.0f, y));
    auto guardEnemy1 = std::make_unique<EnemyTank>(guardX1, guardY1, EntityType::EnemyLight);
    guardEnemy1->setProjectileCallback([this](float px, float py, float ang, int dmg, bool isPlayer) {
        this->spawnProjectile(std::make_unique<Projectile>(px, py, ang, dmg, isPlayer));
    });
    guardEnemy1->setPlayerPosition(player->getPosition());
    guardEnemy1->setPlayerBasePosition(playerBase->getPosition());
    
    std::vector<Entity*> guardTargets1;
    guardTargets1.push_back(player.get());
    guardTargets1.push_back(playerBase.get());
    for (auto& ally : allies) {
        guardTargets1.push_back(ally.get());
    }
    guardEnemy1->setAllyTargets(guardTargets1);
    enemies.push_back(std::move(guardEnemy1));
    
    float guardX2 = std::max(50.0f, std::min(extendedWidth - 50.0f, x - 80.0f));
    float guardY2 = std::max(50.0f, std::min(static_cast<float>(screenSize.y) - 50.0f, y));
    auto guardEnemy2 = std::make_unique<EnemyTank>(guardX2, guardY2, EntityType::EnemyLight);
    guardEnemy2->setProjectileCallback([this](float px, float py, float ang, int dmg, bool isPlayer) {
        this->spawnProjectile(std::make_unique<Projectile>(px, py, ang, dmg, isPlayer));
    });
    guardEnemy2->setPlayerPosition(player->getPosition());
    guardEnemy2->setPlayerBasePosition(playerBase->getPosition());
    
    std::vector<Entity*> guardTargets2;
    guardTargets2.push_back(player.get());
    guardTargets2.push_back(playerBase.get());
    for (auto& ally : allies) {
        guardTargets2.push_back(ally.get());
    }
    guardEnemy2->setAllyTargets(guardTargets2);
    enemies.push_back(std::move(guardEnemy2));
}

Game::~Game() {}

void Game::run() {
    while (window.isOpen()) {
        processEvents();
        
        if (state == GameState::Playing) {
            float deltaTime = clock.restart().asSeconds();
            if (deltaTime > 0.1f) deltaTime = 0.016f;
            update(deltaTime);
        }
        
        render();
    }
}

void Game::processEvents() {
    while (auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            return;
        }
        
        if (auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            player->handleInput(keyPressed->code, true);
            
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                window.close();
            }
            
            if (keyPressed->code == sf::Keyboard::Key::Num1) selectedAllyIndex = 0;
            if (keyPressed->code == sf::Keyboard::Key::Num2) selectedAllyIndex = 1;
            if (keyPressed->code == sf::Keyboard::Key::Num3) selectedAllyIndex = 2;
            if (keyPressed->code == sf::Keyboard::Key::Space) selectedAllyIndex = -1;
            
            if (keyPressed->code == sf::Keyboard::Key::LShift || keyPressed->code == sf::Keyboard::Key::RShift) {
                shiftPressed = true;
            }
            
            if (keyPressed->code == sf::Keyboard::Key::F || keyPressed->code == sf::Keyboard::Key::Enter) {
                firePlayerWeapon();
            }
        }
        
        if (auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
            player->handleInput(keyReleased->code, false);
            
            if (keyReleased->code == sf::Keyboard::Key::LShift || keyReleased->code == sf::Keyboard::Key::RShift) {
                shiftPressed = false;
            }
        }
        
        if (auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mousePressed->button == sf::Mouse::Button::Left) {
                sf::Vector2f worldPos = window.mapPixelToCoords(mousePressed->position);
                
                bool clickedAlly = false;
                selectedAllyIndex = -1;
                for (size_t i = 0; i < allies.size(); ++i) {
                    sf::FloatRect bounds = allies[i]->getBounds();
                    if (bounds.contains(worldPos)) {
                        selectedAllyIndex = static_cast<int>(i);
                        clickedAlly = true;
                        break;
                    }
                }
                
                if (!clickedAlly) {
                    firePlayerWeapon();
                }
            }
            
            if (mousePressed->button == sf::Mouse::Button::Right) {
                sf::Vector2f worldPos = window.mapPixelToCoords(mousePressed->position);
                handleAllyCommand(worldPos);
            }
        }
    }
}

void Game::firePlayerWeapon() {
    if (!player || !player->isAlive()) return;
    
    player->shoot();
    
    float turretAngle = player->getTurretRotation();
    
    if (std::isnan(turretAngle) || std::isinf(turretAngle)) {
        turretAngle = 0.0f;
    }
    
    sf::Vector2f spawnPos = player->getPosition();
    float rad = turretAngle * 3.14159f / 180.0f;
    spawnPos.x += std::cos(rad) * 30.0f;
    spawnPos.y += std::sin(rad) * 30.0f;
    
    auto proj = std::make_unique<Projectile>(
        spawnPos.x, spawnPos.y, turretAngle, player->getDamage(), true);
    projectiles.push_back(std::move(proj));
}

void Game::handleAllyCommand(sf::Vector2f worldPos) {
    if (allies.empty()) return;
    
    bool clickedOnEnemy = false;
    Entity* targetEnemy = nullptr;
    
    for (auto& enemy : enemies) {
        if (!enemy) continue;
        sf::FloatRect bounds = enemy->getBounds();
        if (bounds.contains(worldPos)) {
            targetEnemy = enemy.get();
            clickedOnEnemy = true;
            break;
        }
    }
    
    if (!clickedOnEnemy) {
        for (auto& fort : forts) {
            if (!fort) continue;
            sf::FloatRect bounds = fort->getBounds();
            if (bounds.contains(worldPos)) {
                targetEnemy = fort.get();
                clickedOnEnemy = true;
                break;
            }
        }
    }
    
    if (shiftPressed) {
        for (auto& ally : allies) {
            if (!ally) continue;
            if (clickedOnEnemy && targetEnemy) {
                ally->setAttackTarget(targetEnemy);
            } else {
                ally->setTargetPosition(worldPos);
            }
        }
    } else if (selectedAllyIndex >= 0 && selectedAllyIndex < static_cast<int>(allies.size())) {
        auto& ally = allies[selectedAllyIndex];
        if (!ally) return;
        if (clickedOnEnemy && targetEnemy) {
            ally->setAttackTarget(targetEnemy);
        } else {
            ally->setTargetPosition(worldPos);
        }
    }
}

void Game::update(float deltaTime) {
    if (std::isnan(deltaTime) || std::isinf(deltaTime) || deltaTime <= 0) {
        deltaTime = 0.016f;
    }
    
    player->setBarriers(&barriers);
    if (player->isInsideBarrier()) {
        player->teleportToRandomPosition();
    }
    player->update(deltaTime);
    
    playerBase->clearAllyTargets();
    for (auto& enemy : enemies) {
        playerBase->addEnemyTarget(enemy.get());
    }
    for (auto& fort : forts) {
        playerBase->addEnemyTarget(fort.get());
    }
    playerBase->setBombCallback([this](float cx, float cy, float radius) {
        this->handleBombExplosion(cx, cy, radius);
    });
    playerBase->update(deltaTime);
    
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    player->aimAtMouse(mousePos);
    
    for (auto& ally : allies) {
        ally->setBarriers(&barriers);
        if (ally->isInsideBarrier()) {
            ally->teleportToRandomPosition();
        }
        ally->setEnemyList(&enemies);
        ally->setPlayerBasePosition(playerBase->getPosition());
        ally->setProjectileCallback([this](float x, float y, float angle, int dmg, bool isPlayer) {
            this->spawnProjectile(std::make_unique<Projectile>(x, y, angle, dmg, isPlayer));
        });
        ally->update(deltaTime);
    }
    
    for (auto& enemy : enemies) {
        enemy->setBarriers(&barriers);
        if (enemy->isInsideBarrier()) {
            enemy->teleportToRandomPosition();
        }
        enemy->setProjectileCallback([this](float x, float y, float angle, int dmg, bool isPlayer) {
            this->spawnProjectile(std::make_unique<Projectile>(x, y, angle, dmg, isPlayer));
        });
        enemy->setPlayerPosition(player->getPosition());
        enemy->setPlayerBasePosition(playerBase->getPosition());
        std::vector<Entity*> allyEntities;
        for (auto& ally : allies) {
            allyEntities.push_back(ally.get());
        }
        enemy->setAllyTargets(allyEntities);
        enemy->update(deltaTime);
    }
    
    for (auto& fort : forts) {
        fort->setProjectileCallback([this](float x, float y, float angle, int dmg, bool isPlayer) {
            this->spawnProjectile(std::make_unique<Projectile>(x, y, angle, dmg, isPlayer));
        });
        fort->setBombCallback([this](float cx, float cy, float radius) {
            this->handleBombExplosion(cx, cy, radius);
        });
        fort->clearAllyTargets();
        fort->addEnemyTarget(playerBase.get());
        fort->addEnemyTarget(player.get());
        for (auto& ally : allies) {
            fort->addEnemyTarget(ally.get());
        }
        fort->update(deltaTime);
    }
    
    for (auto& proj : projectiles) {
        proj->update(deltaTime);
    }
    
    heartSpawnTimer += deltaTime;
    if (heartSpawnTimer > 10.0f) {
        heartSpawnTimer = 0.0f;
        sf::Vector2u screenSize = window.getSize();
        float x = 100.0f + static_cast<float>(std::rand() % static_cast<int>(screenSize.x - 200));
        float y = 100.0f + static_cast<float>(std::rand() % static_cast<int>(screenSize.y - 200));
        spawnHeart(x, y);
    }
    
    for (auto& heart : hearts) {
        heart->update(deltaTime);
    }
    
    for (auto it = hearts.begin(); it != hearts.end(); ) {
        if (player->getBounds().findIntersection((*it)->getBounds()).has_value()) {
            player->heal((*it)->getHealAmount());
            particles.emitExplosion((*it)->getPosition());
            it = hearts.erase(it);
        } else if ((*it)->isMarkedForDeletion()) {
            it = hearts.erase(it);
        } else {
            ++it;
        }
    }
    
    particles.update(deltaTime);
    
    checkCollisions();
    
    if (!playerBase->isAlive()) {
        state = GameState::Defeat;
    }
    
    int totalEnemies = enemies.size() + forts.size();
    hud.update(waveManager.getWaveNumber(), score, totalEnemies);
    hud.setPlayerHealth(player->getHealth(), player->getMaxHealth());
    hud.setSelectedAlly(selectedAllyIndex);
    
    if (totalEnemies == 0 && !waveManager.isGameComplete()) {
        waveManager.startNextWave();
        
        for (int i = 0; i < 2; i++) {
            sf::Vector2u screenSize = window.getSize();
            float x = 200.0f + static_cast<float>(std::rand() % static_cast<int>(screenSize.x - 400));
            float y = 200.0f + static_cast<float>(std::rand() % static_cast<int>(screenSize.y - 400));
            auto ally = std::make_unique<AllyTank>(x, y, allies.size());
            allies.push_back(std::move(ally));
        }
        
        auto newEnemies = waveManager.spawnEnemies(&barriers);
        for (auto& enemy : newEnemies) {
            enemies.push_back(std::move(enemy));
        }
        auto newForts = waveManager.spawnForts(&barriers);
        for (auto& fort : newForts) {
            forts.push_back(std::move(fort));
        }
        spawnEnemyBase();
    }
    
    if (waveManager.isGameComplete()) {
        state = GameState::Victory;
    }
    
    if (!player->isAlive()) {
        state = GameState::Defeat;
    }
}

void Game::render() {
    sf::Vector2u windowSize = window.getSize();
    window.clear(sf::Color(45, 74, 28));
    
    int tilesX = (windowSize.x + 99) / 100;
    int tilesY = (windowSize.y + 99) / 100;
    
    for (int i = 0; i < tilesX; i++) {
        for (int j = 0; j < tilesY; j++) {
            sf::RectangleShape grass(sf::Vector2f(100, 100));
            grass.setPosition({static_cast<float>(i * 100), static_cast<float>(j * 100)});
            grass.setFillColor(sf::Color(45 + (i + j) % 10, 74 + (i + j) % 10, 28 + (i + j) % 5));
            window.draw(grass);
        }
    }
    
    for (const auto& barrier : barriers) {
        sf::RectangleShape rect(barrier.size);
        rect.setPosition(barrier.position);
        rect.setFillColor(sf::Color(80, 60, 40));
        rect.setOutlineColor(sf::Color(50, 40, 25));
        rect.setOutlineThickness(2);
        window.draw(rect);
    }
    
    playerBase->render(window);
    
    sf::Vector2f basePos = playerBase->getPosition();
    sf::RectangleShape baseHealthBg({120, 12});
    baseHealthBg.setOrigin({60, 6});
    baseHealthBg.setPosition({basePos.x, basePos.y - 80});
    baseHealthBg.setFillColor(sf::Color(50, 50, 50));
    window.draw(baseHealthBg);
    
    float baseHealthRatio = static_cast<float>(playerBase->getHealth()) / playerBase->getMaxHealth();
    sf::RectangleShape baseHealthBar({110 * baseHealthRatio, 8});
    baseHealthBar.setOrigin({55, 4});
    baseHealthBar.setPosition({basePos.x, basePos.y - 80});
    baseHealthBar.setFillColor(sf::Color(0, 200, 0));
    window.draw(baseHealthBar);
    
    for (auto& fort : forts) {
        fort->render(window);
    }
    
    for (auto& enemy : enemies) {
        enemy->render(window);
    }
    
    for (auto& ally : allies) {
        ally->render(window);
    }
    
    player->render(window);
    
    for (auto& proj : projectiles) {
        proj->render(window);
    }
    
    for (auto& heart : hearts) {
        heart->render(window);
    }
    
    particles.render(window);
    
    hud.render(window);
    
    if (state == GameState::Victory) {
        sf::RectangleShape overlay(sf::Vector2f(windowSize.x, windowSize.y));
        overlay.setFillColor(sf::Color(0, 0, 0, 180));
        window.draw(overlay);
        
        sf::Text victoryText(hud.getFont(), "VICTORY!", 72);
        victoryText.setFillColor(sf::Color(255, 215, 0));
        victoryText.setPosition({windowSize.x / 2.0f - 150, windowSize.y / 2.0f - 50});
        window.draw(victoryText);
        
        sf::Text scoreText(hud.getFont(), "Final Score: " + std::to_string(score), 36);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition({windowSize.x / 2.0f - 100, windowSize.y / 2.0f + 30});
        window.draw(scoreText);
    }
    
    if (state == GameState::Defeat) {
        sf::RectangleShape overlay(sf::Vector2f(windowSize.x, windowSize.y));
        overlay.setFillColor(sf::Color(0, 0, 0, 180));
        window.draw(overlay);
        
        sf::Text defeatText(hud.getFont(), "DEFEATED", 72);
        defeatText.setFillColor(sf::Color(255, 69, 0));
        defeatText.setPosition({windowSize.x / 2.0f - 150, windowSize.y / 2.0f - 50});
        window.draw(defeatText);
        
        sf::Text scoreText(hud.getFont(), "Final Score: " + std::to_string(score), 36);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition({windowSize.x / 2.0f - 100, windowSize.y / 2.0f + 30});
        window.draw(scoreText);
    }
    
    window.display();
}

void Game::spawnProjectile(std::unique_ptr<Projectile> projectile) {
    projectiles.push_back(std::move(projectile));
}

void Game::spawnHeart(float x, float y) {
    hearts.push_back(std::make_unique<Heart>(x, y, 25));
}

void Game::handleBombExplosion(float cx, float cy, float radius) {
    for (int i = 0; i < 20; i++) {
        float angle = static_cast<float>(i) * 3.14159f * 2.0f / 20.0f;
        float dist = radius * 0.8f;
        float ex = cx + std::cos(angle) * dist;
        float ey = cy + std::sin(angle) * dist;
        particles.emitExplosion({ex, ey});
    }
    
    particles.emitExplosion({cx, cy});
    particles.emitExplosion({cx - radius * 0.3f, cy - radius * 0.3f});
    particles.emitExplosion({cx + radius * 0.3f, cy - radius * 0.3f});
    particles.emitExplosion({cx - radius * 0.3f, cy + radius * 0.3f});
    particles.emitExplosion({cx + radius * 0.3f, cy + radius * 0.3f});
    particles.emitExplosion({cx - radius * 0.6f, cy});
    particles.emitExplosion({cx + radius * 0.6f, cy});
    particles.emitExplosion({cx, cy - radius * 0.6f});
    particles.emitExplosion({cx, cy + radius * 0.6f});
    
    const int bombDamage = 500;
    
    for (auto& ally : allies) {
        if (ally && ally->isAlive()) {
            float dist = std::sqrt(std::pow(ally->getPosition().x - cx, 2) + 
                                  std::pow(ally->getPosition().y - cy, 2));
            if (dist <= radius) {
                ally->takeDamage(bombDamage);
            }
        }
    }
    
    for (auto& enemy : enemies) {
        if (enemy && enemy->isAlive()) {
            float dist = std::sqrt(std::pow(enemy->getPosition().x - cx, 2) + 
                                  std::pow(enemy->getPosition().y - cy, 2));
            if (dist <= radius) {
                enemy->takeDamage(bombDamage);
            }
        }
    }
    
    if (player && player->isAlive()) {
        float dist = std::sqrt(std::pow(player->getPosition().x - cx, 2) + 
                              std::pow(player->getPosition().y - cy, 2));
        if (dist <= radius) {
            player->takeDamage(bombDamage);
        }
    }
    
    if (playerBase && playerBase->isAlive()) {
        float dist = std::sqrt(std::pow(playerBase->getPosition().x - cx, 2) + 
                              std::pow(playerBase->getPosition().y - cy, 2));
        if (dist <= radius) {
            playerBase->takeDamage(bombDamage);
        }
    }
}

bool intersects(const sf::FloatRect& a, const sf::FloatRect& b) {
    return a.findIntersection(b).has_value();
}

void Game::checkCollisions() {
    auto projIt = projectiles.begin();
    while (projIt != projectiles.end()) {
        bool hit = false;
        auto& proj = *projIt;
        
        if (proj->isPlayerProjectile()) {
            for (size_t j = 0; j < enemies.size(); ++j) {
                if (enemies[j] && enemies[j]->isAlive() && 
                    intersects(proj->getBounds(), enemies[j]->getBounds())) {
                    enemies[j]->takeDamage(proj->getDamage());
                    particles.emitExplosion(proj->getPosition());
                    if (!enemies[j]->isAlive()) {
                        enemies.erase(enemies.begin() + j);
                        score += 100;
                        waveManager.enemyDestroyed();
                    }
                    hit = true;
                    break;
                }
            }
            
            if (!hit) {
                for (size_t j = 0; j < forts.size(); ++j) {
                    if (forts[j] && forts[j]->isAlive() && 
                        intersects(proj->getBounds(), forts[j]->getBounds())) {
                        forts[j]->takeDamage(proj->getDamage());
                        particles.emitExplosion(proj->getPosition());
                        if (!forts[j]->isAlive()) {
                            forts.erase(forts.begin() + j);
                            score += 200;
                            waveManager.enemyDestroyed();
                        }
                        hit = true;
                        break;
                    }
                }
            }
            
            if (!hit) {
                for (const auto& barrier : barriers) {
                    if (intersects(proj->getBounds(), barrier)) {
                        particles.emitExplosion(proj->getPosition());
                        hit = true;
                        break;
                    }
                }
            }
        } else {
            for (size_t j = 0; j < enemies.size(); ++j) {
                if (enemies[j] && enemies[j]->isAlive() && 
                    intersects(proj->getBounds(), enemies[j]->getBounds())) {
                    hit = true;
                    break;
                }
            }
            
            if (!hit && player && player->isAlive() && 
                intersects(proj->getBounds(), player->getBounds())) {
                player->takeDamage(proj->getDamage());
                particles.emitExplosion(proj->getPosition());
                hit = true;
            }
            
            if (!hit && playerBase && playerBase->isAlive() && 
                intersects(proj->getBounds(), playerBase->getBounds())) {
                playerBase->takeDamage(proj->getDamage());
                particles.emitExplosion(proj->getPosition());
                hit = true;
            }
            
            if (!hit) {
                for (size_t k = 0; k < allies.size(); ++k) {
                    if (allies[k] && allies[k]->isAlive() && 
                        intersects(proj->getBounds(), allies[k]->getBounds())) {
                        allies[k]->takeDamage(proj->getDamage());
                        particles.emitExplosion(proj->getPosition());
                        if (!allies[k]->isAlive()) {
                            allies.erase(allies.begin() + k);
                        }
                        hit = true;
                        break;
                    }
                }
            }
            
            if (!hit) {
                for (size_t k = 0; k < forts.size(); ++k) {
                    if (forts[k] && forts[k]->isAlive() && 
                        intersects(proj->getBounds(), forts[k]->getBounds())) {
                        forts[k]->takeDamage(proj->getDamage());
                        particles.emitExplosion(proj->getPosition());
                        if (!forts[k]->isAlive()) {
                            forts.erase(forts.begin() + k);
                            score += 200;
                            waveManager.enemyDestroyed();
                        }
                        hit = true;
                        break;
                    }
                }
            }
            
            if (!hit) {
                for (const auto& barrier : barriers) {
                    if (intersects(proj->getBounds(), barrier)) {
                        particles.emitExplosion(proj->getPosition());
                        hit = true;
                        break;
                    }
                }
            }
        }
        
        if (hit || proj->isMarkedForDeletion()) {
            projIt = projectiles.erase(projIt);
        } else {
            ++projIt;
        }
    }
}
