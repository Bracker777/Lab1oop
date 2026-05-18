#pragma once

#include <SDL.h>

#include <string>

struct MonsterStats {
    enum class Behavior {
        ChaseWander,
        PatrolTurn
    };

    float moveSpeed = 1.1F;
    float collisionRadius = 0.24F;
    float maxHealth = 100.0F;
    float aggroRange = 3.5F;
    float meleeRange = 0.85F;
    int meleeDamage = 7;
    float attackCooldownSeconds = 0.9F;
    float turnSpeed = 2.2F;
    float wanderAmplitude = 0.28F;
    Behavior behavior = Behavior::ChaseWander;
};

class MonsterType {
public:
    MonsterType() = default;
    MonsterType(std::string id, std::string texturePath, MonsterStats stats);

    const std::string& getId() const;
    const std::string& getTexturePath() const;
    const MonsterStats& getStats() const;

    bool ensureTextureLoaded(SDL_Renderer* renderer);
    SDL_Texture* getTexture() const;
    void unloadTexture();

private:
    std::string id_;
    std::string texturePath_;
    MonsterStats stats_{};
    SDL_Texture* texture_ = nullptr;
};

class Monster {
public:
    Monster() = default;
    Monster(const MonsterType* type, float x, float y, float headingRad, float wanderSeed);

    const MonsterType* getType() const;
    float getX() const;
    float getY() const;
    float getHeading() const;
    float getHealth() const;
    float getAttackCooldown() const;
    float getWanderSeed() const;

    void setX(float value);
    void setY(float value);
    void setHeading(float value);
    void setAttackCooldown(float value);

    bool isAlive() const;
    void resetHealth();
    float applyDamage(float amount);

private:
    const MonsterType* type_ = nullptr;
    float x_ = 0.0F;
    float y_ = 0.0F;
    float headingRad_ = 0.0F;
    float health_ = 0.0F;
    float attackCooldown_ = 0.0F;
    float wanderSeed_ = 0.0F;
};
