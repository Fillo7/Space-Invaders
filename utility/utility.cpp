#include <cmath>
#include <fstream>
#include <sstream>
#include "utility.h"

namespace Invaders
{

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

void saveHighScore(const std::string& filename, const uint32_t highScore)
{
    std::ofstream outputFile(filename, std::ios::trunc | std::ios_base::out);

    if (!outputFile.is_open())
    {
        return;
    }
    outputFile << highScore << std::endl;
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
            result += 25;
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

uint32_t getBonusScore(const uint32_t time)
{
    uint32_t timeInSeconds = time / SECOND_DURATION;
    uint32_t twoMinutes = 120;

    if (timeInSeconds > twoMinutes)
    {
        return 0;
    }
    else
    {
        return 3 * (twoMinutes - timeInSeconds);
    }
}

} // namespace Invaders
