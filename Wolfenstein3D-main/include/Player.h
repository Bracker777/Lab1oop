#pragma once

#include <SDL.h>

class Map;

class Player {
public:
    Player();

    void update(const Uint8* keyboardState, float deltaTimeSeconds, const Map& map);
    void reset(float x, float y, float angleRad);

    float getX() const;
    float getY() const;
    float getAngle() const;
    float getFov() const;

private:
    float x_;
    float y_;
    float angleRad_;
    float fovRad_;
    float moveSpeed_;
    float turnSpeed_;
    float collisionRadius_;
};
