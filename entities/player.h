#pragma once

#include <cstdint>
#include "../lib/leetlib.h"

namespace Invaders
{

// Player class represents a player spaceship. Handles movement, status and shooting.
// Player collider is implemented as a rectangle.
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
        active(true),
        bulletCooldown(0)
    {}

    void update(const uint32_t currentTime)
    {
        if (!active)
        {
            return;
        }

        if (bulletCooldown > 0)
        {
            bulletCooldown--;
        }
        if (!IsKeyDown(VK_SPACE))
        {
            bulletCooldown = 0;
        }

        if (IsKeyDown(VK_LEFT) || IsKeyDown('A'))
        {
            x -= 7.0f;
        }
        else if (IsKeyDown(VK_RIGHT) || IsKeyDown('D'))
        {
            x += 7.0f;
        }

        x = Clamp(x, size, 800.0f - size);
        DrawSprite(sprite, x, y, size, size, PI + sin(currentTime * 0.1f) * 0.1f, 0xffffffff);
    }

    bool isShooting() const
    {
        if (active && IsKeyDown(VK_SPACE) && bulletCooldown <= 0)
        {
            bulletCooldown = 15;
            return true;
        }
        return false;
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
    mutable int bulletCooldown;
};

} // namespace Invaders
