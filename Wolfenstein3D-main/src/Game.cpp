#include "Game.h"

#include <SDL.h>
#include <SDL_image.h>
#include <algorithm>
#include <cmath>
#include <vector>

namespace {
constexpr float kPi = 3.14159265358979323846F;
constexpr float normalizeAngle(float value) {
    while (value > kPi) {
        value -= 2.0F * kPi;
    }
    while (value < -kPi) {
        value += 2.0F * kPi;
    }
    return value;
}

constexpr int kMagSize = 12;
constexpr int kMaxReserveAmmo = 60;
constexpr float kReloadDurationSeconds = 1.05F;
}

Game::Game()
    : window_(nullptr),
      renderer_(nullptr),
      weaponTexture_(nullptr),
      wallTexture_(nullptr),
      doorTexture_(nullptr),
      isRunning_(false),
      interactRequested_(false),
      shootRequested_(false),
      restartRequested_(false),
      reloadRequested_(false),
      levelComplete_(false),
      campaignComplete_(false),
      reloading_(false),
      gameOver_(false),
      currentLevelIndex_(0),
      playerHealth_(100),
      ammoInMag_(kMagSize),
      ammoReserve_(kMaxReserveAmmo),
      kills_(0),
      shootCooldown_(0.0F),
      shootFlashTimer_(0.0F),
      reloadTimer_(0.0F) {}

Game::~Game() {
    shutdown();
}

bool Game::initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return false;
    }
    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0) {
        return false;
    }

    window_ = SDL_CreateWindow(
        "Wolfenstein 3D MVP",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        kWindowWidth,
        kWindowHeight,
        SDL_WINDOW_SHOWN
    );

    if (!window_) {
        return false;
    }

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_) {
        return false;
    }

    weaponTexture_ = IMG_LoadTexture(renderer_, "assets/weapon.png");
    wallTexture_ = IMG_LoadTexture(renderer_, "assets/wall.png");
    doorTexture_ = IMG_LoadTexture(renderer_, "assets/door.png");

    registerMonsterTypes();

    levels_ = {
        {{"111111111111",
          "100000000001",
          "101101210101",
          "100100010001",
          "101100020101",
          "100001010001",
          "101111011101",
          "100000000301",
          "111111111111"},
         {{"grunt", 5.5F, 1.8F, 1.1F, 0.2F},
          {"grunt", 8.2F, 3.5F, -0.8F, 1.0F},
          {"brute", 6.7F, 6.5F, 2.4F, 2.1F},
          {"tank", 4.5F, 1.5F, 1.5707963F, 4.2F}},
         1.5F,
         1.5F,
         0.0F},
        {{"111111111111",
          "100000000001",
          "101111111101",
          "100000000001",
          "101011111101",
          "102010000001",
          "101011111101",
          "100000000301",
          "111111111111"},
         {{"grunt", 4.6F, 3.5F, 0.0F, 0.5F},
          {"fast", 8.4F, 3.2F, 0.0F, 1.7F},
          {"grunt", 5.8F, 6.4F, 0.0F, 2.7F},
          {"fast", 9.2F, 6.2F, 0.0F, 3.4F}},
         1.5F,
         1.5F,
         0.0F}};
    loadLevel(0);

    isRunning_ = true;
    return true;
}

void Game::run() {
    constexpr float kFixedDeltaSeconds = 1.0F / 120.0F;
    constexpr float kMaxFrameTimeSeconds = 0.1F;

    Uint64 previousCounter = SDL_GetPerformanceCounter();
    float accumulator = 0.0F;

    while (isRunning_) {
        const Uint64 currentCounter = SDL_GetPerformanceCounter();
        const float elapsedSeconds = static_cast<float>(currentCounter - previousCounter) /
                                     static_cast<float>(SDL_GetPerformanceFrequency());
        previousCounter = currentCounter;

        accumulator += (elapsedSeconds > kMaxFrameTimeSeconds) ? kMaxFrameTimeSeconds : elapsedSeconds;

        processEvents();
        while (accumulator >= kFixedDeltaSeconds) {
            update(kFixedDeltaSeconds);
            accumulator -= kFixedDeltaSeconds;
        }
        render();
    }
}

void Game::shutdown() {
    if (doorTexture_) {
        SDL_DestroyTexture(doorTexture_);
        doorTexture_ = nullptr;
    }
    if (wallTexture_) {
        SDL_DestroyTexture(wallTexture_);
        wallTexture_ = nullptr;
    }
    if (weaponTexture_) {
        SDL_DestroyTexture(weaponTexture_);
        weaponTexture_ = nullptr;
    }
    for (MonsterType& type : monsterTypes_) {
        type.unloadTexture();
    }
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }

    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }

    IMG_Quit();
    SDL_Quit();
    isRunning_ = false;
}

void Game::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning_ = false;
        } else if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
            isRunning_ = false;
        } else if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_E && !event.key.repeat) {
            interactRequested_ = true;
        } else if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_SPACE && !event.key.repeat) {
            shootRequested_ = true;
        } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
            shootRequested_ = true;
        } else if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_R && !event.key.repeat) {
            reloadRequested_ = true;
        } else if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_RETURN && !event.key.repeat) {
            restartRequested_ = true;
        }
    }
}

void Game::update(const float deltaTimeSeconds) {
    if (restartRequested_) {
        if (gameOver_) {
            loadLevel(currentLevelIndex_);
        } else if (levelComplete_) {
            if (currentLevelIndex_ + 1 < static_cast<int>(levels_.size())) {
                loadLevel(currentLevelIndex_ + 1);
            } else {
                campaignComplete_ = true;
                levelComplete_ = false;
            }
        } else if (campaignComplete_) {
            loadLevel(0);
            campaignComplete_ = false;
            kills_ = 0;
        }
        restartRequested_ = false;
    }

    const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);
    if (!gameOver_ && !levelComplete_ && !campaignComplete_) {
        player_.update(keyboardState, deltaTimeSeconds, map_);
        updateMonsters(deltaTimeSeconds);
        handleReload(deltaTimeSeconds);
        handleShooting(deltaTimeSeconds);

        if (getAliveMonstersCount() == 0) {
            const int playerCellX = static_cast<int>(std::floor(player_.getX()));
            const int playerCellY = static_cast<int>(std::floor(player_.getY()));
            if (map_.getCell(playerCellX, playerCellY) == '3') {
                levelComplete_ = true;
            }
        }
    } else {
        shootRequested_ = false;
        reloadRequested_ = false;
        reloading_ = false;
    }

    if (interactRequested_) {
        map_.openDoorAhead(player_.getX(), player_.getY(), player_.getAngle(), 1.3F);
        interactRequested_ = false;
    }
}

void Game::updateMonsters(const float deltaTimeSeconds) {
    for (Monster& monster : monsters_) {
        if (!monster.isAlive()) {
            continue;
        }

        const MonsterType* type = monster.getType();
        if (type == nullptr) {
            continue;
        }
        const MonsterStats& stats = type->getStats();

        monster.setAttackCooldown(std::max(0.0F, monster.getAttackCooldown() - deltaTimeSeconds));

        const float toPlayerX = player_.getX() - monster.getX();
        const float toPlayerY = player_.getY() - monster.getY();
        const float toPlayerDistance2 = toPlayerX * toPlayerX + toPlayerY * toPlayerY;
        const float toPlayerDistance = std::sqrt(toPlayerDistance2);
        const float chaseAngle = std::atan2(toPlayerY, toPlayerX);

        if (toPlayerDistance < stats.meleeRange && monster.getAttackCooldown() <= 0.0F) {
            playerHealth_ = std::max(0, playerHealth_ - stats.meleeDamage);
            monster.setAttackCooldown(stats.attackCooldownSeconds);
            if (playerHealth_ <= 0) {
                gameOver_ = true;
                reloading_ = false;
            }
        }

        if (stats.behavior == MonsterStats::Behavior::ChaseWander) {
            const float chaseMix = std::clamp((stats.aggroRange - toPlayerDistance) / stats.aggroRange, 0.0F, 1.0F);
            const float wanderTerm = stats.wanderAmplitude *
                                     std::sin(static_cast<float>(SDL_GetTicks()) * 0.001F + monster.getWanderSeed());
            monster.setHeading(normalizeAngle((1.0F - chaseMix) * (monster.getHeading() + wanderTerm * deltaTimeSeconds) + chaseMix * chaseAngle));
        }

        const float desiredX = monster.getX() + std::cos(monster.getHeading()) * stats.moveSpeed * deltaTimeSeconds;
        const float desiredY = monster.getY() + std::sin(monster.getHeading()) * stats.moveSpeed * deltaTimeSeconds;

        bool moved = false;
        if (map_.canMoveTo(desiredX, monster.getY(), stats.collisionRadius)) {
            monster.setX(desiredX);
            moved = true;
        }
        if (map_.canMoveTo(monster.getX(), desiredY, stats.collisionRadius)) {
            monster.setY(desiredY);
            moved = true;
        }

        if (!moved) {
            if (stats.behavior == MonsterStats::Behavior::PatrolTurn) {
                monster.setHeading(normalizeAngle(monster.getHeading() + (kPi * 0.5F)));
            } else {
                monster.setHeading(monster.getHeading() + stats.turnSpeed * deltaTimeSeconds);
            }
        }
    }
}

void Game::handleReload(const float deltaTimeSeconds) {
    if (reloading_) {
        reloadTimer_ = std::max(0.0F, reloadTimer_ - deltaTimeSeconds);
        if (reloadTimer_ <= 0.0F) {
            const int needed = kMagSize - ammoInMag_;
            const int toLoad = std::min(needed, ammoReserve_);
            ammoInMag_ += toLoad;
            ammoReserve_ -= toLoad;
            reloading_ = false;
        }
        return;
    }

    if (!reloadRequested_) {
        return;
    }
    reloadRequested_ = false;

    if (ammoInMag_ >= kMagSize || ammoReserve_ <= 0) {
        return;
    }

    reloading_ = true;
    reloadTimer_ = kReloadDurationSeconds;
}

void Game::handleShooting(const float deltaTimeSeconds) {
    constexpr float kShootCooldownSeconds = 0.22F;
    constexpr float kShootRange = 9.0F;
    constexpr float kHitConeRad = 0.11F;
    constexpr float kHitDamage = 40.0F;

    shootCooldown_ = std::max(0.0F, shootCooldown_ - deltaTimeSeconds);
    shootFlashTimer_ = std::max(0.0F, shootFlashTimer_ - deltaTimeSeconds);

    if (!shootRequested_ || shootCooldown_ > 0.0F || reloading_) {
        shootRequested_ = false;
        return;
    }

    shootRequested_ = false;
    shootCooldown_ = kShootCooldownSeconds;
    if (ammoInMag_ <= 0) {
        shootFlashTimer_ = 0.0F;
        return;
    }
    ammoInMag_ -= 1;
    shootFlashTimer_ = 0.08F;

    const float px = player_.getX();
    const float py = player_.getY();
    const float pa = player_.getAngle();

    Monster* bestTarget = nullptr;
    float bestDistance = 1.0e30F;

    for (Monster& monster : monsters_) {
        if (!monster.isAlive()) {
            continue;
        }

        const float dx = monster.getX() - px;
        const float dy = monster.getY() - py;
        const float distance = std::sqrt(dx * dx + dy * dy);
        if (distance > kShootRange || distance < 0.001F) {
            continue;
        }

        const float angleToMonster = std::atan2(dy, dx);
        const float angleDiff = std::abs(normalizeAngle(angleToMonster - pa));
        if (angleDiff > kHitConeRad) {
            continue;
        }

        bool blocked = false;
        constexpr int kRaySteps = 40;
        for (int step = 1; step < kRaySteps; ++step) {
            const float t = static_cast<float>(step) / static_cast<float>(kRaySteps);
            const float sx = px + dx * t;
            const float sy = py + dy * t;
            if (map_.isWallAt(sx, sy)) {
                blocked = true;
                break;
            }
        }
        if (blocked) {
            continue;
        }

        if (distance < bestDistance) {
            bestDistance = distance;
            bestTarget = &monster;
        }
    }

    if (bestTarget != nullptr) {
        const float previousHealth = bestTarget->getHealth();
        bestTarget->applyDamage(kHitDamage);
        if (previousHealth > 0.0F && !bestTarget->isAlive()) {
            kills_ += 1;
        }
    }
}

void Game::loadLevel(const int levelIndex) {
    if (levelIndex < 0 || levelIndex >= static_cast<int>(levels_.size())) {
        return;
    }
    currentLevelIndex_ = levelIndex;

    const LevelData& level = levels_[static_cast<std::size_t>(levelIndex)];
    map_.loadGrid(level.grid);
    monsters_.clear();
    monsters_.reserve(level.spawns.size());
    for (const LevelData::MonsterSpawn& spawn : level.spawns) {
        const MonsterType* type = getMonsterType(spawn.typeId);
        if (type == nullptr) {
            continue;
        }
        const_cast<MonsterType*>(type)->ensureTextureLoaded(renderer_);
        monsters_.emplace_back(type, spawn.x, spawn.y, spawn.headingRad, spawn.wanderSeed);
    }

    auto findNearestFreeCellCenter = [this](int cellX, int cellY, float radius) {
        auto isFree = [this, radius](int x, int y) {
            const char c = map_.getCell(x, y);
            if (c != '0' && c != '3') {
                return false;
            }
            const float wx = static_cast<float>(x) + 0.5F;
            const float wy = static_cast<float>(y) + 0.5F;
            return map_.canMoveTo(wx, wy, radius);
        };

        if (isFree(cellX, cellY)) {
            return std::pair<float, float>{static_cast<float>(cellX) + 0.5F, static_cast<float>(cellY) + 0.5F};
        }

        constexpr int kMaxRadius = 8;
        for (int r = 1; r <= kMaxRadius; ++r) {
            for (int dy = -r; dy <= r; ++dy) {
                for (int dx = -r; dx <= r; ++dx) {
                    if (std::abs(dx) != r && std::abs(dy) != r) {
                        continue;
                    }
                    const int nx = cellX + dx;
                    const int ny = cellY + dy;
                    if (isFree(nx, ny)) {
                        return std::pair<float, float>{static_cast<float>(nx) + 0.5F, static_cast<float>(ny) + 0.5F};
                    }
                }
            }
        }

        return std::pair<float, float>{static_cast<float>(cellX) + 0.5F, static_cast<float>(cellY) + 0.5F};
    };

    for (Monster& monster : monsters_) {
        const float spawnRadius = (monster.getType() != nullptr) ? monster.getType()->getStats().collisionRadius : 0.24F;
        const int cellX = static_cast<int>(std::floor(monster.getX()));
        const int cellY = static_cast<int>(std::floor(monster.getY()));
        if (!map_.canMoveTo(monster.getX(), monster.getY(), spawnRadius) || map_.isWall(cellX, cellY)) {
            const auto [fx, fy] = findNearestFreeCellCenter(cellX, cellY, spawnRadius);
            monster.setX(fx);
            monster.setY(fy);
        }
    }
    player_.reset(level.spawnX, level.spawnY, level.spawnAngle);

    playerHealth_ = 100;
    ammoInMag_ = kMagSize;
    ammoReserve_ = kMaxReserveAmmo;
    shootCooldown_ = 0.0F;
    shootFlashTimer_ = 0.0F;
    reloadTimer_ = 0.0F;
    levelComplete_ = false;
    gameOver_ = false;
    reloading_ = false;
    reloadRequested_ = false;
}

int Game::getAliveMonstersCount() const {
    int alive = 0;
    for (const Monster& monster : monsters_) {
        if (monster.isAlive()) {
            alive += 1;
        }
    }
    return alive;
}

void Game::render() {
    const int halfHeight = kWindowHeight / 2;
    const float lightIntensity = 0.82F + 0.18F * std::sin(static_cast<float>(SDL_GetTicks()) * 0.0028F);

    SDL_SetRenderDrawColor(
        renderer_,
        static_cast<Uint8>(28.0F * lightIntensity),
        static_cast<Uint8>(38.0F * lightIntensity),
        static_cast<Uint8>(54.0F * lightIntensity),
        255);
    SDL_RenderClear(renderer_);

    SDL_Rect floorRect{0, halfHeight, kWindowWidth, kWindowHeight - halfHeight};
    SDL_SetRenderDrawColor(
        renderer_,
        static_cast<Uint8>(45.0F * lightIntensity),
        static_cast<Uint8>(45.0F * lightIntensity),
        static_cast<Uint8>(45.0F * lightIntensity),
        255);
    SDL_RenderFillRect(renderer_, &floorRect);

    raycaster_.render(renderer_, map_, player_, monsters_, wallTexture_, doorTexture_, lightIntensity, kWindowWidth, kWindowHeight);
    renderPostEffects(lightIntensity);
    renderMinimap();
    renderHud();
    SDL_RenderPresent(renderer_);
}

void Game::renderMinimap() const {
    constexpr int kCellSize = 12;
    constexpr int kPadding = 12;

    const int mapWidth = map_.getWidth();
    const int mapHeight = map_.getHeight();
    const SDL_Rect bgRect{
        kPadding - 4,
        kPadding - 4,
        mapWidth * kCellSize + 8,
        mapHeight * kCellSize + 8};

    SDL_SetRenderDrawColor(renderer_, 10, 10, 10, 180);
    SDL_RenderFillRect(renderer_, &bgRect);

    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            const char cell = map_.getCell(x, y);
            SDL_Rect cellRect{
                kPadding + x * kCellSize,
                kPadding + y * kCellSize,
                kCellSize - 1,
                kCellSize - 1};

            if (cell == '1') {
                SDL_SetRenderDrawColor(renderer_, 160, 160, 160, 255);
            } else if (cell == '2') {
                SDL_SetRenderDrawColor(renderer_, 170, 115, 65, 255);
            } else if (cell == '3') {
                SDL_SetRenderDrawColor(renderer_, 60, 180, 75, 255);
            } else {
                SDL_SetRenderDrawColor(renderer_, 35, 35, 35, 255);
            }
            SDL_RenderFillRect(renderer_, &cellRect);
        }
    }

    for (const Monster& monster : monsters_) {
        if (!monster.isAlive()) {
            continue;
        }
        SDL_Rect monsterRect{
            kPadding + static_cast<int>(monster.getX() * static_cast<float>(kCellSize)) - 2,
            kPadding + static_cast<int>(monster.getY() * static_cast<float>(kCellSize)) - 2,
            5,
            5};
        SDL_SetRenderDrawColor(renderer_, 210, 55, 55, 255);
        SDL_RenderFillRect(renderer_, &monsterRect);
    }

    const int px = kPadding + static_cast<int>(player_.getX() * static_cast<float>(kCellSize));
    const int py = kPadding + static_cast<int>(player_.getY() * static_cast<float>(kCellSize));
    SDL_Rect playerRect{px - 2, py - 2, 5, 5};
    SDL_SetRenderDrawColor(renderer_, 255, 70, 70, 255);
    SDL_RenderFillRect(renderer_, &playerRect);

    const int lookX = px + static_cast<int>(std::cos(player_.getAngle()) * static_cast<float>(kCellSize));
    const int lookY = py + static_cast<int>(std::sin(player_.getAngle()) * static_cast<float>(kCellSize));
    SDL_SetRenderDrawColor(renderer_, 255, 120, 120, 255);
    SDL_RenderDrawLine(renderer_, px, py, lookX, lookY);
}

void Game::renderHud() const {
    const int cx = kWindowWidth / 2;
    const int cy = kWindowHeight / 2;

    SDL_SetRenderDrawColor(renderer_, 245, 245, 245, 230);
    SDL_RenderDrawLine(renderer_, cx - 8, cy, cx + 8, cy);
    SDL_RenderDrawLine(renderer_, cx, cy - 8, cx, cy + 8);

    if (weaponTexture_) {
        int texW = 0;
        int texH = 0;
        SDL_QueryTexture(weaponTexture_, nullptr, nullptr, &texW, &texH);
        const int targetW = 300;
        const int targetH = (texW > 0) ? (texH * targetW / texW) : 160;
        SDL_Rect dst{
            (kWindowWidth - targetW) / 2,
            kWindowHeight - targetH,
            targetW,
            targetH};
        SDL_SetTextureBlendMode(weaponTexture_, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(weaponTexture_, gameOver_ ? 120 : 255);
        SDL_RenderCopy(renderer_, weaponTexture_, nullptr, &dst);
    } else {
        SDL_Rect fallbackGun{cx - 70, kWindowHeight - 60, 140, 50};
        SDL_SetRenderDrawColor(renderer_, 70, 70, 78, 220);
        SDL_RenderFillRect(renderer_, &fallbackGun);
    }

    const SDL_Rect hpBg{20, kWindowHeight - 34, 204, 16};
    SDL_SetRenderDrawColor(renderer_, 25, 25, 25, 220);
    SDL_RenderFillRect(renderer_, &hpBg);

    const int clampedHp = std::clamp(playerHealth_, 0, 100);
    SDL_Rect hpFill{22, kWindowHeight - 32, clampedHp * 2, 12};
    SDL_SetRenderDrawColor(renderer_, static_cast<Uint8>(220 - clampedHp), static_cast<Uint8>(40 + clampedHp * 2), 45, 240);
    SDL_RenderFillRect(renderer_, &hpFill);

    const SDL_Rect ammoBg{kWindowWidth - 224, kWindowHeight - 34, 204, 16};
    SDL_SetRenderDrawColor(renderer_, 25, 25, 25, 220);
    SDL_RenderFillRect(renderer_, &ammoBg);

    const int ammoWidth = static_cast<int>((static_cast<float>(std::clamp(ammoInMag_, 0, kMagSize)) / static_cast<float>(kMagSize)) * 200.0F);
    SDL_Rect ammoFill{kWindowWidth - 222, kWindowHeight - 32, ammoWidth, 12};
    SDL_SetRenderDrawColor(renderer_, 70, 140, 220, 240);
    SDL_RenderFillRect(renderer_, &ammoFill);

    const int reservePips = std::clamp(ammoReserve_ / 6, 0, 10);
    for (int i = 0; i < reservePips; ++i) {
        SDL_Rect pip{kWindowWidth - 224 + i * 20, kWindowHeight - 50, 14, 8};
        SDL_SetRenderDrawColor(renderer_, 120, 170, 255, 220);
        SDL_RenderFillRect(renderer_, &pip);
    }

    const int aliveMonsters = getAliveMonstersCount();
    for (int i = 0; i < aliveMonsters; ++i) {
        SDL_Rect enemyDot{20 + i * 10, 14, 7, 7};
        SDL_SetRenderDrawColor(renderer_, 220, 55, 55, 245);
        SDL_RenderFillRect(renderer_, &enemyDot);
    }

    for (int i = 0; i <= currentLevelIndex_; ++i) {
        SDL_Rect lvlPip{kWindowWidth - 20 - (i * 14), 14, 10, 10};
        SDL_SetRenderDrawColor(renderer_, 90, 200, 255, 245);
        SDL_RenderFillRect(renderer_, &lvlPip);
    }

    if (shootFlashTimer_ > 0.0F) {
        SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer_, 255, 245, 210, 35);
        SDL_Rect flashRect{0, 0, kWindowWidth, kWindowHeight};
        SDL_RenderFillRect(renderer_, &flashRect);
    }

    if (gameOver_) {
        SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 120);
        SDL_Rect overlay{0, 0, kWindowWidth, kWindowHeight};
        SDL_RenderFillRect(renderer_, &overlay);

        SDL_Rect bar{180, kWindowHeight / 2 - 20, 440, 40};
        SDL_SetRenderDrawColor(renderer_, 150, 20, 20, 210);
        SDL_RenderFillRect(renderer_, &bar);
    } else if (levelComplete_) {
        SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 85);
        SDL_Rect overlay{0, 0, kWindowWidth, kWindowHeight};
        SDL_RenderFillRect(renderer_, &overlay);

        SDL_Rect bar{180, kWindowHeight / 2 - 20, 440, 40};
        SDL_SetRenderDrawColor(renderer_, 20, 145, 60, 220);
        SDL_RenderFillRect(renderer_, &bar);
    } else if (campaignComplete_) {
        SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 85);
        SDL_Rect overlay{0, 0, kWindowWidth, kWindowHeight};
        SDL_RenderFillRect(renderer_, &overlay);

        SDL_Rect bar{160, kWindowHeight / 2 - 22, 480, 44};
        SDL_SetRenderDrawColor(renderer_, 25, 90, 180, 220);
        SDL_RenderFillRect(renderer_, &bar);
    }

    if (reloading_) {
        const float progress = std::clamp(1.0F - (reloadTimer_ / kReloadDurationSeconds), 0.0F, 1.0F);
        SDL_Rect reloadBg{(kWindowWidth / 2) - 90, kWindowHeight - 78, 180, 10};
        SDL_SetRenderDrawColor(renderer_, 20, 20, 20, 220);
        SDL_RenderFillRect(renderer_, &reloadBg);
        SDL_Rect reloadFill{(kWindowWidth / 2) - 88, kWindowHeight - 76, static_cast<int>(176.0F * progress), 6};
        SDL_SetRenderDrawColor(renderer_, 245, 200, 80, 230);
        SDL_RenderFillRect(renderer_, &reloadFill);
    }
}

void Game::renderPostEffects(const float lightIntensity) const {
    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);

    for (int y = 0; y < kWindowHeight; y += 2) {
        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 18);
        SDL_RenderDrawLine(renderer_, 0, y, kWindowWidth, y);
    }

    constexpr int kBand = 42;
    SDL_Rect top{0, 0, kWindowWidth, kBand};
    SDL_Rect bottom{0, kWindowHeight - kBand, kWindowWidth, kBand};
    SDL_Rect left{0, 0, kBand, kWindowHeight};
    SDL_Rect right{kWindowWidth - kBand, 0, kBand, kWindowHeight};

    const Uint8 vignetteAlpha = static_cast<Uint8>(std::clamp(56.0F - lightIntensity * 20.0F, 22.0F, 62.0F));
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, vignetteAlpha);
    SDL_RenderFillRect(renderer_, &top);
    SDL_RenderFillRect(renderer_, &bottom);
    SDL_RenderFillRect(renderer_, &left);
    SDL_RenderFillRect(renderer_, &right);
}

void Game::registerMonsterTypes() {
    monsterTypes_.clear();

    MonsterStats grunt{};
    grunt.moveSpeed = 1.1F;
    grunt.maxHealth = 100.0F;
    grunt.meleeDamage = 7;
    grunt.aggroRange = 3.5F;

    MonsterStats fast = grunt;
    fast.moveSpeed = 1.25F;
    fast.collisionRadius = 0.22F;
    fast.maxHealth = 70.0F;
    fast.meleeDamage = 5;

    MonsterStats brute = grunt;
    brute.moveSpeed = 0.85F;
    brute.collisionRadius = 0.28F;
    brute.maxHealth = 160.0F;
    brute.meleeDamage = 11;
    brute.attackCooldownSeconds = 1.1F;

    MonsterStats tank = grunt;
    tank.moveSpeed = 0.75F;
    tank.collisionRadius = 0.38F;
    tank.maxHealth = 240.0F;
    tank.meleeDamage = 14;
    tank.attackCooldownSeconds = 1.25F;
    tank.behavior = MonsterStats::Behavior::PatrolTurn;

    monsterTypes_.emplace_back("grunt", "assets/monster.png", grunt);
    monsterTypes_.emplace_back("fast", "assets/monster_fast.png", fast);
    monsterTypes_.emplace_back("brute", "assets/monster_brute.png", brute);
    monsterTypes_.emplace_back("tank", "assets/monster_tank.png", tank);

    for (MonsterType& type : monsterTypes_) {
        type.ensureTextureLoaded(renderer_);
    }
}

const MonsterType* Game::getMonsterType(const std::string& id) const {
    for (const MonsterType& type : monsterTypes_) {
        if (type.getId() == id) {
            return &type;
        }
    }
    return nullptr;
}
