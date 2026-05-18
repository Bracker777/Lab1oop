#include "Raycaster.h"

#include "Map.h"
#include "Player.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace {
Uint8 shadeChannel(const int value, const float factor) {
    const int scaled = static_cast<int>(static_cast<float>(value) * factor);
    return static_cast<Uint8>(std::clamp(scaled, 0, 255));
}
}

void Raycaster::render(
    SDL_Renderer* renderer,
    const Map& map,
    const Player& player,
    const std::vector<Monster>& monsters,
    SDL_Texture* wallTexture,
    SDL_Texture* doorTexture,
    const float lightIntensity,
    const int screenWidth,
    const int screenHeight) const {
    const float playerX = player.getX();
    const float playerY = player.getY();
    const float angle = player.getAngle();
    const float fov = player.getFov();

    const float dirX = std::cos(angle);
    const float dirY = std::sin(angle);
    const float cameraPlaneScale = std::tan(fov * 0.5F);
    const float planeX = -dirY * cameraPlaneScale;
    const float planeY = dirX * cameraPlaneScale;
    std::vector<float> depthBuffer(static_cast<std::size_t>(screenWidth), 1.0e30F);

    for (int x = 0; x < screenWidth; ++x) {
        const float cameraX = (2.0F * static_cast<float>(x) / static_cast<float>(screenWidth)) - 1.0F;
        const float rayDirX = dirX + planeX * cameraX;
        const float rayDirY = dirY + planeY * cameraX;

        int mapX = static_cast<int>(playerX);
        int mapY = static_cast<int>(playerY);

        const float deltaDistX = (rayDirX == 0.0F) ? 1.0e30F : std::abs(1.0F / rayDirX);
        const float deltaDistY = (rayDirY == 0.0F) ? 1.0e30F : std::abs(1.0F / rayDirY);

        int stepX = 0;
        int stepY = 0;
        float sideDistX = 0.0F;
        float sideDistY = 0.0F;

        if (rayDirX < 0.0F) {
            stepX = -1;
            sideDistX = (playerX - static_cast<float>(mapX)) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (static_cast<float>(mapX + 1) - playerX) * deltaDistX;
        }
        if (rayDirY < 0.0F) {
            stepY = -1;
            sideDistY = (playerY - static_cast<float>(mapY)) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (static_cast<float>(mapY + 1) - playerY) * deltaDistY;
        }

        bool hit = false;
        int side = 0;
        char hitCell = '1';
        while (!hit) {
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }

            hitCell = map.getCell(mapX, mapY);
            if (hitCell != '0') {
                hit = true;
            }
        }

        float perpWallDist = 0.0F;
        if (side == 0) {
            perpWallDist = (static_cast<float>(mapX) - playerX + static_cast<float>(1 - stepX) * 0.5F) / rayDirX;
        } else {
            perpWallDist = (static_cast<float>(mapY) - playerY + static_cast<float>(1 - stepY) * 0.5F) / rayDirY;
        }

        perpWallDist = std::max(perpWallDist, 0.0001F);
        depthBuffer[static_cast<std::size_t>(x)] = perpWallDist;
        const int lineHeight = static_cast<int>(static_cast<float>(screenHeight) / perpWallDist);
        const int drawStart = std::max(0, (screenHeight - lineHeight) / 2);
        const int drawEnd = std::min(screenHeight - 1, (screenHeight + lineHeight) / 2);

        float wallX = 0.0F;
        if (side == 0) {
            wallX = playerY + perpWallDist * rayDirY;
        } else {
            wallX = playerX + perpWallDist * rayDirX;
        }
        wallX -= std::floor(wallX);

        float distanceShade = 1.0F / (1.0F + perpWallDist * perpWallDist * 0.08F);
        distanceShade = std::clamp(distanceShade, 0.2F, 1.0F);
        if (side == 1) {
            distanceShade *= 0.8F;
        }

        SDL_Texture* wallTex = (hitCell == '2') ? doorTexture : wallTexture;
        int wallTexW = 0;
        int wallTexH = 0;
        const bool hasWallTexture =
            (wallTex != nullptr) &&
            (SDL_QueryTexture(wallTex, nullptr, nullptr, &wallTexW, &wallTexH) == 0) &&
            wallTexW > 0 && wallTexH > 0;

        if (hasWallTexture) {
            const int srcX = std::clamp(static_cast<int>(wallX * static_cast<float>(wallTexW - 1)), 0, wallTexW - 1);
            SDL_Rect src{srcX, 0, 1, wallTexH};
            SDL_Rect dst{x, drawStart, 1, std::max(1, drawEnd - drawStart + 1)};
            const Uint8 texShade = shadeChannel(255, distanceShade * lightIntensity);
            SDL_SetTextureBlendMode(wallTex, SDL_BLENDMODE_BLEND);
            SDL_SetTextureColorMod(wallTex, texShade, texShade, texShade);
            SDL_RenderCopy(renderer, wallTex, &src, &dst);
        } else {
            int baseR = 150;
            int baseG = 150;
            int baseB = 150;
            if (hitCell == '2') {
                baseR = 150;
                baseG = 105;
                baseB = 60;
            }

            for (int y = drawStart; y <= drawEnd; ++y) {
                const int texX = static_cast<int>(wallX * 16.0F);
                const int texY = (y + (lineHeight / 3)) / 12;
                const bool checker = ((texX + texY) & 1) == 0;
                const float patternShade = checker ? 0.88F : 1.08F;
                const float finalShade = std::clamp(distanceShade * patternShade * lightIntensity, 0.15F, 1.2F);

                SDL_SetRenderDrawColor(
                    renderer,
                    shadeChannel(baseR, finalShade),
                    shadeChannel(baseG, finalShade),
                    shadeChannel(baseB, finalShade),
                    255
                );
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
    }

    struct DrawMonster {
        float distance;
        float x;
        float y;
        const Monster* source;
    };
    std::vector<DrawMonster> drawMonsters;
    drawMonsters.reserve(monsters.size());
    for (const Monster& monster : monsters) {
        if (!monster.isAlive()) {
            continue;
        }
        const float dx = monster.getX() - playerX;
        const float dy = monster.getY() - playerY;
        drawMonsters.push_back({dx * dx + dy * dy, monster.getX(), monster.getY(), &monster});
    }
    std::sort(drawMonsters.begin(), drawMonsters.end(), [](const DrawMonster& a, const DrawMonster& b) {
        return a.distance > b.distance;
    });

    for (const DrawMonster& monster : drawMonsters) {
        const float spriteX = monster.x - playerX;
        const float spriteY = monster.y - playerY;
        const float invDet = 1.0F / (planeX * dirY - dirX * planeY);
        const float transformX = invDet * (dirY * spriteX - dirX * spriteY);
        const float transformY = invDet * (-planeY * spriteX + planeX * spriteY);
        if (transformY <= 0.1F) {
            continue;
        }

        const int spriteScreenX = static_cast<int>((screenWidth * 0.5F) * (1.0F + transformX / transformY));
        const int spriteHeight = std::abs(static_cast<int>(screenHeight / transformY));
        const int spriteWidth = spriteHeight;
        const int drawStartY = std::max(0, (screenHeight - spriteHeight) / 2);
        const int drawEndY = std::min(screenHeight - 1, (screenHeight + spriteHeight) / 2);
        const int drawStartX = std::max(0, spriteScreenX - spriteWidth / 2);
        const int drawEndX = std::min(screenWidth - 1, spriteScreenX + spriteWidth / 2);

        const float distanceShade = std::clamp((1.0F / (1.0F + transformY * transformY * 0.2F)) * lightIntensity, 0.25F, 1.0F);
        const Uint8 bodyR = shadeChannel(195, distanceShade);
        const Uint8 bodyG = shadeChannel(52, distanceShade);
        const Uint8 bodyB = shadeChannel(52, distanceShade);
        const Uint8 eye = shadeChannel(220, distanceShade);
        SDL_Texture* monsterTexture = nullptr;
        if (monster.source != nullptr && monster.source->getType() != nullptr) {
            monsterTexture = monster.source->getType()->getTexture();
        }
        int texW = 0;
        int texH = 0;
        const bool hasTexture = (monsterTexture != nullptr) &&
                                (SDL_QueryTexture(monsterTexture, nullptr, nullptr, &texW, &texH) == 0) &&
                                texW > 0 && texH > 0;
        if (hasTexture) {
            SDL_SetTextureBlendMode(monsterTexture, SDL_BLENDMODE_BLEND);
            SDL_SetTextureColorMod(monsterTexture, shadeChannel(255, distanceShade), shadeChannel(255, distanceShade), shadeChannel(255, distanceShade));
        }

        for (int stripe = drawStartX; stripe <= drawEndX; ++stripe) {
            if (transformY >= depthBuffer[static_cast<std::size_t>(stripe)]) {
                continue;
            }

            const float normalizedX = static_cast<float>(stripe - drawStartX) / static_cast<float>(std::max(1, drawEndX - drawStartX));
            if (hasTexture) {
                const int srcX = std::clamp(static_cast<int>(normalizedX * static_cast<float>(texW - 1)), 0, texW - 1);
                SDL_Rect src{srcX, 0, 1, texH};
                SDL_Rect dst{stripe, drawStartY, 1, std::max(1, drawEndY - drawStartY + 1)};
                SDL_RenderCopy(renderer, monsterTexture, &src, &dst);
            } else {
                for (int y = drawStartY; y <= drawEndY; ++y) {
                    const float normalizedY = static_cast<float>(y - drawStartY) / static_cast<float>(std::max(1, drawEndY - drawStartY));
                    const float dx = normalizedX - 0.5F;
                    const float dy = normalizedY - 0.5F;
                    const float radius2 = dx * dx + dy * dy;
                    if (radius2 > 0.24F) {
                        continue;
                    }

                    Uint8 r = bodyR;
                    Uint8 g = bodyG;
                    Uint8 b = bodyB;

                    if (normalizedY < 0.28F && std::abs(dx) < 0.12F) {
                        r = eye;
                        g = eye;
                        b = eye;
                    } else if (normalizedY > 0.62F) {
                        r = shadeChannel(static_cast<int>(r), 0.75F);
                        g = shadeChannel(static_cast<int>(g), 0.75F);
                        b = shadeChannel(static_cast<int>(b), 0.75F);
                    }

                    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
                    SDL_RenderDrawPoint(renderer, stripe, y);
                }
            }
        }
    }
}
