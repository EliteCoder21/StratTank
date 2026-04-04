#pragma once

#include "Tank.h"
#include <memory>
#include <vector>

class Entity;
class EnemyTank;
class Fort;

class AllyTank : public Tank {
public:
    AllyTank(float x, float y, int allyId);

    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

    void setAttackTarget(Entity* target);
    void clearTarget();
    void followPlayer();
    void setAutoTargetEnabled(bool enabled);
    void setGuardingBase(bool guarding) { guardingBase = guarding; }
    void setEnemyList(const std::vector<std::unique_ptr<EnemyTank>>* list);
    void setFortList(const std::vector<std::unique_ptr<Fort>>* list);
    void setPlayerBasePosition(sf::Vector2f pos) { playerBasePosition = pos; }
    bool hasTarget() const { return attackTarget != nullptr; }
    int getAllyId() const { return allyId; }

private:
    int allyId;
    Entity* attackTarget = nullptr;
    float followDistance = 100.0f;
    float aiUpdateTimer;
    bool autoTargetEnabled;
    const std::vector<std::unique_ptr<EnemyTank>>* enemyList = nullptr;
    const std::vector<std::unique_ptr<Fort>>* fortList = nullptr;
    sf::Vector2f playerBasePosition;
    sf::Vector2f patrolTarget;
    float patrolRadius = 100.0f;
    bool isPatrolling = false;
    bool guardingBase = false;
    
    void findAndAttackNearestEnemy();
    void defendBase();
    void wander();
    void attackEntity(Entity* target);
    void renderBody(sf::RenderWindow& window) override;
    void renderTurret(sf::RenderWindow& window) override;
};
