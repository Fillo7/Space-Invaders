#include <array>
#include <cmath>
#include <cstdint>
#include <list>
#include <map>
#include "lib/leetlib.h"

class Player
{
public:
    Player() :
        Player(nullptr)
    {}

    Player(void* sprite) :
        x(400.0f),
        y(550.0f),
        size(50.0f),
        sprite(sprite)
    {}

    void update(const uint32_t currentTime)
    {
        if (IsKeyDown(VK_LEFT))
        {
            x -= 7.0f;
        }
        else if (IsKeyDown(VK_RIGHT))
        {
            x += 7.0f;
        }

        x = max(size, min(800.0f - size, x));
        DrawSprite(sprite, x, y, size, size, PI + sin(currentTime * 0.1f) * 0.1f, 0xffffffff);
    }

    float getX() const
    {
        return x;
    }

    float getY() const
    {
        return y;
    }

    float getSize() const
    {
        return size;
    }

    void* getSprite()
    {
        return sprite;
    }

private:
    float x;
    float y;
    float size;
    void* sprite;
};

class Bullet
{
public:
    Bullet() :
        Bullet(0.0f, 0.0f, nullptr, 0)
    {}

    Bullet(const float x, const float y, void* sprite, const uint32_t timeSnapshot) :
        x(x),
        y(y),
        size(10.0f),
        angle(0.0f),
        timeSnapshot(timeSnapshot),
        aliveTimer(0),
        timeToLive(6 * 60),
        sprite(sprite)
    {}

    void update(const uint32_t currentTime)
    {
        uint32_t deltaTime = currentTime - timeSnapshot;
        aliveTimer += deltaTime;
        timeSnapshot = currentTime;

        y -= 4.0f;
        angle += 0.1f;
        DrawSprite(sprite, x, y, size, size, angle, 0xffffffff);
    }

    bool isExpired() const
    {
        return aliveTimer >= timeToLive;
    }

    float getX() const
    {
        return x;
    }

    float getY() const
    {
        return y;
    }

    float getSize() const
    {
        return size;
    }

    float getAngle() const
    {
        return angle;
    }

    void* getSprite()
    {
        return sprite;
    }

private:
    float x;
    float y;
    float size;
    float angle;
    uint32_t timeSnapshot;
    uint32_t aliveTimer;
    uint32_t timeToLive;
    void* sprite;
};

class Enemy
{
public:
    Enemy() :
        x(0.0f),
        y(0.0f),
        size(0.0f),
        sprite(nullptr),
        order(0),
        active(true),
        enraged(false)
    {}

    void update(const uint32_t currentTime)
    {
        if (!active)
        {
            return;
        }

        if (!enraged)
        {
            magicAI(currentTime);
        }
        else
        {
            enragedAI();
        }
    }

    void initialize(const float x, const float y, const float size, void* sprite, const int order)
    {
        this->x = x;
        this->y = y;
        this->size = size;
        this->sprite = sprite;
        this->order = order;
    }

    void enrage()
    {
        this->enraged = true;
    }

    float getX() const
    {
        return x;
    }

    float getY() const
    {
        return y;
    }

    float getSize() const
    {
        return size;
    }

    void* getSprite()
    {
        return sprite;
    }

private:
    float x;
    float y;
    float size;
    void* sprite;
    int order;
    bool active;
    bool enraged;

    void magicAI(const uint32_t currentTime)
    {
        float xo = 0;
        float yo = 0;
        int n1 = int(currentTime) + order * order + order * order * order;
        int n2 = int(currentTime) + order + order * order + 3 * order * order * order;

        if (((n1 >> 6) & 0x7) == 0x7)
        {
            xo += (1.0f - cos((n1 & 0x7f) / 64.0f * 2.0f * PI)) * (20 + ((order * order) % 9));
        }
        if (((n1 >> 6) & 0x7) == 0x7)
        {
            yo += (sin((n1 & 0x7f) / 64.0f * 2.0f * PI)) * (20 + ((order * order) % 9));
        }
        if (((n2 >> 8) & 0xf) == 0xf)
        {
            yo += (1.0f - cos((n2 & 0xff) / 256.0f * 2.0f * PI)) * (150 + ((order * order) % 9));
        }

        DrawSprite(sprite, x + xo, y + yo, float((10 + (order % 17))), float((10 + (order % 17))), 0.0f, 0xffffffff);
    }

    void enragedAI()
    {
        // todo
        DrawSprite(sprite, x, y, float((10 + (order % 17))), float((10 + (order % 17))), 0.0f, 0xffff0000);
    }
};

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
        enemies[i].initialize((i % 10) * 60.0f + 120.0f, (i / 10) * 60.0f + 70.0f, float((10 + (i % 17))), enemySprite, i);
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
