#pragma once

#include <SDL.h>
#include <string>
#include <vector>

#include "Map.h"
#include "Monster.h"
#include "Player.h"
#include "Raycaster.h"

class Game {
public:
    Game();
    ~Game();

    bool initialize();
    void run();
    void shutdown();

private:
    struct LevelData {
        std::vector<std::string> grid;
        struct MonsterSpawn {
            std::string typeId;
            float x;
            float y;
            float headingRad;
            float wanderSeed;
        };
        std::vector<MonsterSpawn> spawns;
        float spawnX;
        float spawnY;
        float spawnAngle;
    };

    void processEvents();
    void update(float deltaTimeSeconds);
    void updateMonsters(float deltaTimeSeconds);
    void handleShooting(float deltaTimeSeconds);
    void handleReload(float deltaTimeSeconds);
    void loadLevel(int levelIndex);
    int getAliveMonstersCount() const;
    void registerMonsterTypes();
    const MonsterType* getMonsterType(const std::string& id) const;
    void renderHud() const;
    void render();
    void renderMinimap() const;
    void renderPostEffects(float lightIntensity) const;

    SDL_Window* window_;
    SDL_Renderer* renderer_;
    SDL_Texture* weaponTexture_;
    SDL_Texture* wallTexture_;
    SDL_Texture* doorTexture_;
    bool isRunning_;
    bool interactRequested_;
    bool shootRequested_;
    bool restartRequested_;
    bool reloadRequested_;
    bool levelComplete_;
    bool campaignComplete_;
    bool reloading_;
    bool gameOver_;
    int currentLevelIndex_;
    int playerHealth_;
    int ammoInMag_;
    int ammoReserve_;
    int kills_;
    float shootCooldown_;
    float shootFlashTimer_;
    float reloadTimer_;
    std::vector<LevelData> levels_;
    std::vector<MonsterType> monsterTypes_;
    std::vector<Monster> monsters_;
    Map map_;
    Player player_;
    Raycaster raycaster_;

    static constexpr int kWindowWidth = 800;
    static constexpr int kWindowHeight = 600;
};
