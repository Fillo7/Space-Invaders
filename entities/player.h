#pragma once

#include <cstdint>
#include "../lib/leetlib.h"

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
