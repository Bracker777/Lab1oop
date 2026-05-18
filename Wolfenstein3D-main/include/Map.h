#pragma once

#include <string>
#include <vector>

class Map {
public:
    Map();
    void loadGrid(const std::vector<std::string>& grid);

    bool isWall(int gridX, int gridY) const;
    char getCell(int gridX, int gridY) const;
    bool isWallAt(float worldX, float worldY) const;
    bool canMoveTo(float worldX, float worldY, float radius) const;
    bool openDoorAhead(float worldX, float worldY, float angleRad, float maxDistance);

    int getWidth() const;
    int getHeight() const;

private:
    std::vector<std::string> grid_;
};
