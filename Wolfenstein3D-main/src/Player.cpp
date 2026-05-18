#include "Player.h"

#include "Map.h"

#include <cmath>

namespace {
constexpr float kPi = 3.14159265358979323846F;
}

Player::Player()
    : x_(1.5F),
      y_(1.5F),
      angleRad_(0.0F),
      fovRad_(70.0F * (kPi / 180.0F)),
      moveSpeed_(3.0F),
      turnSpeed_(2.5F),
      collisionRadius_(0.2F) {}

void Player::update(const Uint8* keyboardState, const float deltaTimeSeconds, const Map& map) {
    float turnInput = 0.0F;
    if (keyboardState[SDL_SCANCODE_LEFT]) {
        turnInput -= 1.0F;
    }
    if (keyboardState[SDL_SCANCODE_RIGHT]) {
        turnInput += 1.0F;
    }

    angleRad_ += turnInput * turnSpeed_ * deltaTimeSeconds;
    if (angleRad_ > kPi) {
        angleRad_ -= 2.0F * kPi;
    } else if (angleRad_ < -kPi) {
        angleRad_ += 2.0F * kPi;
    }

    float moveForward = 0.0F;
    float moveStrafe = 0.0F;

    if (keyboardState[SDL_SCANCODE_W]) {
        moveForward += 1.0F;
    }
    if (keyboardState[SDL_SCANCODE_S]) {
        moveForward -= 1.0F;
    }
    if (keyboardState[SDL_SCANCODE_D]) {
        moveStrafe += 1.0F;
    }
    if (keyboardState[SDL_SCANCODE_A]) {
        moveStrafe -= 1.0F;
    }

    const float forwardX = std::cos(angleRad_);
    const float forwardY = std::sin(angleRad_);
    const float rightX = -forwardY;
    const float rightY = forwardX;
    const float moveStep = moveSpeed_ * deltaTimeSeconds;

    const float desiredX = x_ + (forwardX * moveForward + rightX * moveStrafe) * moveStep;
    const float desiredY = y_ + (forwardY * moveForward + rightY * moveStrafe) * moveStep;

    if (map.canMoveTo(desiredX, y_, collisionRadius_)) {
        x_ = desiredX;
    }
    if (map.canMoveTo(x_, desiredY, collisionRadius_)) {
        y_ = desiredY;
    }
}

void Player::reset(const float x, const float y, const float angleRad) {
    x_ = x;
    y_ = y;
    angleRad_ = angleRad;
}

float Player::getX() const {
    return x_;
}

float Player::getY() const {
    return y_;
}

float Player::getAngle() const {
    return angleRad_;
}

float Player::getFov() const {
    return fovRad_;
}
