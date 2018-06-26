#include <array>
#include <cmath>
#include <cstdint>
#include <list>
#include <map>
#include "lib/leetlib.h"
#include "entities/bullet.h"
#include "entities/enemy.h"
#include "entities/player.h"

void updateEnemies(std::array<Enemy, 50>& enemies, const uint32_t currentTime)
{
    for (int i = 0; i < 50; i++)
    {
        enemies[i].update(currentTime);
    }
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

void Game()
{
    // Setup
    std::map<std::string, void*> spriteMap = getSpriteMap();
    void* enemySprite = spriteMap.find("enemy")->second;
    void* playerSprite = spriteMap.find("player")->second;
    void* bulletSprite = spriteMap.find("bullet")->second;
    const std::string titleString("space invaders");

    uint32_t time = 0;
    Player player(playerSprite);
    std::list<Bullet> activeBullets;
    std::array<Enemy, 50> enemies;

    for (int i = 0; i < 50; i++)
    {
        enemies[i].initialize((i % 10) * 60.0f + 120.0f, (i / 10) * 60.0f + 70.0f, float((10 + (i % 17))), enemySprite, i, &player);
    }

    // Game loop
    while (!WantQuit() && !IsKeyDown(VK_ESCAPE))
    {
        time++;

        // Update movement, AI and collisions
        player.update(time);
        updateEnemies(enemies, time);
        updateBullets(activeBullets, time);

        // Player fire
        static int bulletCooldown = 0;
        if (bulletCooldown > 0)
        {
            bulletCooldown--;
        }
        if (!IsKeyDown(VK_SPACE))
        {
            bulletCooldown = 0;
        }
        if (IsKeyDown(VK_SPACE) && bulletCooldown == 0)
        {
            activeBullets.emplace_back(player.getX(), player.getY(), bulletSprite, time);
            bulletCooldown = 15;
        }

        // Title text
        for (int i = 0; i < int(titleString.length()); i++)
        {
            if (i != 5)
            {
                DrawSprite(spriteMap.find(std::string("") + titleString[i])->second, float(i * 40 + 150), 30.0f, 20.0f, 20.0f, float((sin(time * 0.1f) * i * 0.01f)));
            }
        }

        Flip();
    }
}
