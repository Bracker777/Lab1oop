#include "Map.h"

#include <cmath>

Map::Map()
    : grid_{
          "111111111111",
          "100000000001",
          "101101210101",
          "100100010001",
          "101100020101",
          "100001010001",
          "101111011101",
          "100000000001",
          "111111111111"} {}

void Map::loadGrid(const std::vector<std::string>& grid) {
    grid_ = grid;
}

bool Map::isWall(const int gridX, const int gridY) const {
    const char cell = getCell(gridX, gridY);
    return cell != '0' && cell != '3';
}

char Map::getCell(const int gridX, const int gridY) const {
    if (gridY < 0 || gridY >= getHeight() || gridX < 0 || gridX >= getWidth()) {
        return '1';
    }
    return grid_[static_cast<std::size_t>(gridY)][static_cast<std::size_t>(gridX)];
}

bool Map::isWallAt(const float worldX, const float worldY) const {
    const int gridX = static_cast<int>(std::floor(worldX));
    const int gridY = static_cast<int>(std::floor(worldY));
    return isWall(gridX, gridY);
}

bool Map::canMoveTo(const float worldX, const float worldY, const float radius) const {
    return !isWallAt(worldX - radius, worldY - radius) &&
           !isWallAt(worldX + radius, worldY - radius) &&
           !isWallAt(worldX - radius, worldY + radius) &&
           !isWallAt(worldX + radius, worldY + radius);
}

bool Map::openDoorAhead(const float worldX, const float worldY, const float angleRad, const float maxDistance) {
    constexpr float kStep = 0.05F;
    const float dirX = std::cos(angleRad);
    const float dirY = std::sin(angleRad);

    for (float distance = 0.2F; distance <= maxDistance; distance += kStep) {
        const float sampleX = worldX + dirX * distance;
        const float sampleY = worldY + dirY * distance;
        const int gridX = static_cast<int>(std::floor(sampleX));
        const int gridY = static_cast<int>(std::floor(sampleY));

        const char cell = getCell(gridX, gridY);
        if (cell == '2') {
            grid_[static_cast<std::size_t>(gridY)][static_cast<std::size_t>(gridX)] = '0';
            return true;
        }
        if (cell == '1') {
            return false;
        }
    }

    return false;
}

int Map::getWidth() const {
    return grid_.empty() ? 0 : static_cast<int>(grid_.front().size());
}

int Map::getHeight() const {
    return static_cast<int>(grid_.size());
}
