#include <array>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include "lib/leetlib.h"
#include "entities/bullet.h"
#include "entities/enemy.h"
#include "entities/player.h"

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

void drawText(const std::string& text, const std::map<std::string, void*>& spriteMap, const uint32_t time, const int gap, const float size,
    const float xOffset, const float yOffset, const bool enableRotation)
{
    for (int i = 0; i < int(text.length()); i++)
    {
        if (i == gap)
        {
            continue;
        }

        float rotation = 0.0f;
        if (enableRotation)
        {
            rotation = float((sin(time * 0.1f) * i * 0.01f));
        }
        DrawSprite(spriteMap.find(std::string("") + text[i])->second, float(i * 2 * size + xOffset), yOffset, size, size, rotation);
    }
}

std::map<std::string, void*> getSpriteMap()
{
    std::map<std::string, void*> result;

    void* enemySprite = LoadSprite("gfx/Little Invader.png");
    result.insert(std::make_pair("enemy", enemySprite));

    void* playerSprite = LoadSprite("gfx/Big Invader.png");
    result.insert(std::make_pair("player", playerSprite));

    void* bulletSprite = LoadSprite("gfx/bullet.png");
    result.insert(std::make_pair("bullet", bulletSprite));

    for (char c = 'a'; int(c) <= int('z'); c++)
    {
        void* letterSprite = LoadSprite(std::string("gfx/") + c + "let.png");
        result.insert(std::make_pair(std::string("") + c, letterSprite));
    }

    for (char c = '0'; int(c) <= int('9'); c++)
    {
        void* numberSprite = LoadSprite(std::string("gfx/num") + c + ".png");
        result.insert(std::make_pair(std::string("") + c, numberSprite));
    }

    return result;
}

uint32_t loadHighScore(const std::string& filename)
{
    std::ifstream inputFile(filename, std::ios_base::in);

    if (!inputFile.is_open())
    {
        return 0;
    }

    std::stringstream stream;
    stream << inputFile.rdbuf();
    uint32_t result = std::stoul(stream.str());
    return result;
}

void saveHighScore(const std::string& filename, const uint32_t highScore)
{
    std::ofstream outputFile(filename, std::ios::trunc | std::ios_base::out);

    if (!outputFile.is_open())
    {
        return;
    }
    outputFile << highScore << std::endl;
}

uint32_t getBonusScore(const uint32_t time)
{
    uint32_t timeInSeconds = time / SECOND_DURATION;
    uint32_t fiveMinutes = 300;

    if (timeInSeconds > fiveMinutes)
    {
        return 0;
    }
    else
    {
        return fiveMinutes - timeInSeconds;
    }
}

uint32_t getCurrentScore(const std::array<Enemy, 50>& enemies, const uint32_t time, const bool includeTimeBonus)
{
    uint32_t result = 0;

    for (int i = 0; i < int(enemies.size()); i++)
    {
        if (enemies[i].isActive())
        {
            continue;
        }

        if (enemies[i].isEnraged())
        {
            result += 20;
        }
        else
        {
            result += 10;
        }
    }

    if (includeTimeBonus)
    {
        result += getBonusScore(time);
    }

    return result;
}

void Game()
{
    // Setup
    std::map<std::string, void*> spriteMap = getSpriteMap();
    void* enemySprite = spriteMap.find("enemy")->second;
    void* playerSprite = spriteMap.find("player")->second;
    void* bulletSprite = spriteMap.find("bullet")->second;

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

        // Update movement, AI and collisions
        player.update(time);
        if (player.isShooting())
        {
            activeBullets.emplace_back(player.getX(), player.getY(), bulletSprite, time, &enemies);
        }

        uint32_t defeatedCount = updateEnemies(enemies, time);
        if (defeatedCount == enemies.size())
        {
            if (victoryTime == 0)
            {
                victoryTime = time;
            }
            victory = true;
        }
        updateBullets(activeBullets, time);

        // Title and ending texts
        drawText("space invaders", spriteMap, time, 5, 20.0f, 150.0f, 30.0f, true);
        if (!player.isActive())
        {
            drawText("game over", spriteMap, time, 4, 30.0f, 165.0f, 300.0f, false);
        }
        else if (victory)
        {
            drawText("victory", spriteMap, time, -1, 30.0f, 215.0f, 300.0f, false);
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
