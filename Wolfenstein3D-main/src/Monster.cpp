#include "Monster.h"

#include <SDL_image.h>

#include <algorithm>

MonsterType::MonsterType(std::string id, std::string texturePath, MonsterStats stats)
    : id_(std::move(id)), texturePath_(std::move(texturePath)), stats_(stats) {}

const std::string& MonsterType::getId() const {
    return id_;
}

const std::string& MonsterType::getTexturePath() const {
    return texturePath_;
}

const MonsterStats& MonsterType::getStats() const {
    return stats_;
}

bool MonsterType::ensureTextureLoaded(SDL_Renderer* renderer) {
    if (texture_ != nullptr) {
        return true;
    }
    if (renderer == nullptr || texturePath_.empty()) {
        return false;
    }
    texture_ = IMG_LoadTexture(renderer, texturePath_.c_str());
    return texture_ != nullptr;
}

SDL_Texture* MonsterType::getTexture() const {
    return texture_;
}

void MonsterType::unloadTexture() {
    if (texture_ != nullptr) {
        SDL_DestroyTexture(texture_);
        texture_ = nullptr;
    }
}

Monster::Monster(const MonsterType* type, const float x, const float y, const float headingRad, const float wanderSeed)
    : type_(type), x_(x), y_(y), headingRad_(headingRad), wanderSeed_(wanderSeed) {
    resetHealth();
}

const MonsterType* Monster::getType() const {
    return type_;
}

float Monster::getX() const {
    return x_;
}

float Monster::getY() const {
    return y_;
}

float Monster::getHeading() const {
    return headingRad_;
}

float Monster::getHealth() const {
    return health_;
}

float Monster::getAttackCooldown() const {
    return attackCooldown_;
}

float Monster::getWanderSeed() const {
    return wanderSeed_;
}

void Monster::setX(const float value) {
    x_ = value;
}

void Monster::setY(const float value) {
    y_ = value;
}

void Monster::setHeading(const float value) {
    headingRad_ = value;
}

void Monster::setAttackCooldown(const float value) {
    attackCooldown_ = value;
}

bool Monster::isAlive() const {
    return health_ > 0.0F;
}

void Monster::resetHealth() {
    health_ = (type_ != nullptr) ? type_->getStats().maxHealth : 0.0F;
    attackCooldown_ = 0.0F;
}

float Monster::applyDamage(const float amount) {
    const float previous = health_;
    health_ = std::max(0.0F, health_ - std::max(0.0F, amount));
    return previous - health_;
}

