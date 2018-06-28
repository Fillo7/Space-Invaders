#include <array>
#include <cmath>
#include <cstdint>
#include <list>
#include <map>
#include <random>
#include <string>
#include <vector>
#include "lib/leetlib.h"
#include "entities/bullet.h"
#include "entities/enemy.h"
#include "entities/player.h"
#include "utility/utility.h"

using namespace Invaders;

uint32_t updateEnemies(std::array<Enemy, 50>& enemies, const uint32_t currentTime)
{
    uint32_t defeatedCount = 0;

    for (int i = 0; i < int(enemies.size()); i++)
    {
        enemies[i].update(currentTime);

        if (!enemies[i].isActive())
        {
            defeatedCount++;
        }
    }

    return defeatedCount;
}

void enrageEnemy(std::array<Enemy, 50>& enemies, const uint32_t currentTime)
{
    std::vector<Enemy*> enrageCandidates;

    for (int i = 0; i < int(enemies.size()); i++)
    {
        if (enemies[i].isActive() && !enemies[i].isTransitioning() && !enemies[i].isEnraged())
        {
            enrageCandidates.push_back(&enemies[i]);
        }
    }

    if (enrageCandidates.size() == 0)
    {
        return;
    }

    float currentTimeFloat = float(currentTime / SECOND_DURATION);
    float twoMinutes = 120.0f;
    float enragedSpeed = 2.0f + (6.0f * min(1.0f, currentTimeFloat / twoMinutes));

    int enrageIndex = currentTime % enrageCandidates.size();
    enrageCandidates[enrageIndex]->enrage(enragedSpeed);
}

void updateBullets(std::list<Bullet>& activeBullets, const uint32_t currentTime)
{
    for (auto iterator = activeBullets.begin(); iterator != activeBullets.end();)
    {
        iterator->update(currentTime);

        if (iterator->isExpired())
        {
            iterator = activeBullets.erase(iterator);
        }
        else
        {
            ++iterator;
        }
    }
}

void Game()
{
    // Setup
    std::map<std::string, void*> spriteMap = getSpriteMap();
    void* enemySprite = spriteMap.find("enemy")->second;
    void* playerSprite = spriteMap.find("player")->second;
    void* bulletSprite = spriteMap.find("bullet")->second;

    std::random_device device;
    std::default_random_engine engine(device());
    std::uniform_int_distribution<int> distribution(3 * SECOND_DURATION, 6 * SECOND_DURATION);
    int enrageTimer = distribution(engine);

    bool victory = false;
    uint32_t highScore = loadHighScore("highscore.score");
    uint32_t time = 0;
    uint32_t victoryTime = 0;

    Player player(playerSprite);
    std::list<Bullet> activeBullets;
    std::array<Enemy, 50> enemies;

    for (int i = 0; i < int(enemies.size()); i++)
    {
        enemies[i].initialize((i % 10) * 60.0f + 120.0f, (i / 10) * 60.0f + 70.0f, float((10 + (i % 17))), enemySprite, i, &player);
    }

    // Main game loop
    while (!WantQuit() && !IsKeyDown(VK_ESCAPE))
    {
        time++;
        enrageTimer--;

        // Update movement, AI and collisions
        player.update(time);
        if (player.isShooting())
        {
            activeBullets.emplace_back(player.getX(), player.getY(), bulletSprite, time, &enemies);
        }

        if (enrageTimer <= 0)
        {
            enrageEnemy(enemies, time);
            enrageTimer = distribution(engine);
        }

        uint32_t defeatedCount = updateEnemies(enemies, time);
        if (!victory && defeatedCount == enemies.size())
        {
            victoryTime = time;
            victory = true;
        }
        updateBullets(activeBullets, time);

        // Title and ending texts
        drawText("space invaders", spriteMap, time, 5, 20.0f, 150.0f, 30.0f, true);
        if (!player.isActive())
        {
            drawText("game over", spriteMap, time, 4, 30.0f, 165.0f, 300.0f, true);
        }
        else if (victory)
        {
            drawText("victory", spriteMap, time, -1, 30.0f, 215.0f, 300.0f, true);
        }

        // Score calculation and text
        uint32_t score;
        if (victory)
        {
            score = getCurrentScore(enemies, victoryTime, true);
        }
        else
        {
            score = getCurrentScore(enemies, 0, false);
        }

        if (score > highScore)
        {
            highScore = score;
        }

        drawText("score " + std::to_string(score), spriteMap, time, 5, 8.0f, 615.0f, 520.0f, false);
        drawText("highscore " + std::to_string(highScore), spriteMap, time, 9, 8.0f, 550.0f, 560.0f, false);

        Flip();
    }

    saveHighScore("highscore.score", highScore);
}
