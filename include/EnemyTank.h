#pragma once

#include "Tank.h"
#include <memory>

class Entity;
class EnemyTank;
class Heart;

class EnemyTank : public Tank {
public:
    EnemyTank(float x, float y, EntityType type);

    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

    void setPlayerPosition(sf::Vector2f pos);
    void setPlayerBasePosition(sf::Vector2f pos) { playerBasePosition = pos; }
    void setAllyTargets(const std::vector<Entity*>& allies);
    void takeDamage(int damage) override;
    void setLeader(EnemyTank* leader) { leaderTank = leader; }
    EnemyTank* getLeader() const { return leaderTank; }
    bool hasLeader() const { return leaderTank != nullptr; }
    void setHeartList(const std::vector<std::unique_ptr<Heart>>* list) { heartList = list; }

private:
    sf::Vector2f playerPosition;
    sf::Vector2f playerBasePosition;
    std::vector<Entity*> allyTargets;
    float aiUpdateTimer = 0.0f;
    float aiUpdateInterval = 0.5f;
    bool isFlanking = false;
    sf::Vector2f flankDirection;
    float strafeTimer = 0.0f;
    sf::Vector2f patrolTarget;
    sf::Vector2f patrolDirection;
    float patrolRadius = 250.0f;
    bool isPatrolling = false;
    bool isFleeing = false;
    float lastCombatTime = 0.0f;
    float healCooldown = 0.0f;
    EnemyTank* leaderTank = nullptr;
    float patrolTimer = 0.0f;
    int patrolState = 0;
    const std::vector<std::unique_ptr<Heart>>* heartList = nullptr;
    bool seekingHeart = false;

    void updateAI(float deltaTime);
    void updatePatrol(float deltaTime);
    void renderBody(sf::RenderWindow& window) override;
    void renderTurret(sf::RenderWindow& window) override;
};