#pragma once

#include <SDL.h>
#include <vector>

#include "Monster.h"

class Map;
class Player;

class Raycaster {
public:
    void render(
        SDL_Renderer* renderer,
        const Map& map,
        const Player& player,
        const std::vector<Monster>& monsters,
        SDL_Texture* wallTexture,
        SDL_Texture* doorTexture,
        float lightIntensity,
        int screenWidth,
        int screenHeight) const;
};
