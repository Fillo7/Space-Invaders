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
        sprite(sprite),
        active(true)
    {}

    void update(const uint32_t currentTime)
    {
        if (!active)
        {
            return;
        }

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

    void destroy()
    {
        active = false;
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

    bool isActive() const
    {
        return active;
    }

private:
    float x;
    float y;
    float size;
    void* sprite;
    bool active;
};
