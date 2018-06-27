#pragma once

#include <cstdint>
#include "../lib/leetlib.h"
#include "enemy.h"

// Bullet class represents a bullet shot by a player. Handles bullet movement, status and collisions between bullet and enemies.
// Bullet collider is implemented as a circle.
class Bullet
{
public:
    Bullet() :
        Bullet(0.0f, 0.0f, nullptr, 0, nullptr)
    {}

    Bullet(const float x, const float y, void* sprite, const uint32_t timeSnapshot, std::array<Enemy, 50>* enemies) :
        x(x),
        y(y),
        size(10.0f),
        angle(0.0f),
        timeSnapshot(timeSnapshot),
        aliveTimer(0),
        timeToLive(6 * SECOND_DURATION),
        sprite(sprite),
        active(true),
        enemies(enemies)
    {}

    void update(const uint32_t currentTime)
    {
        if (!active)
        {
            return;
        }

        if (enemies != nullptr)
        {
            for (int i = 0; i < int(enemies->size()); i++)
            {
                if (enemies->at(i).isActive() && intersectsEnemy(enemies->at(i)))
                {
                    enemies->at(i).destroy();
                    destroy();
                    return;
                }
            }
        }

        uint32_t deltaTime = currentTime - timeSnapshot;
        aliveTimer += deltaTime;
        timeSnapshot = currentTime;

        y -= 4.0f;
        angle += 0.1f;
        DrawSprite(sprite, x, y, size, size, angle, 0xffffffff);
    }

    void destroy()
    {
        active = false;
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

    bool isActive() const
    {
        return active;
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
    bool active;
    std::array<Enemy, 50>* enemies;

    // Checks for collision between a bullet and an enemy. Bullet collider is implemented as a circle, enemy collider is implemented as a rectangle.
    bool intersectsEnemy(const Enemy& enemy) const
    {
        float enemyLeft = enemy.getX() - enemy.getSize() / 2.0f;
        float enemyRight = enemy.getX() + enemy.getSize() / 2.0f;
        float enemyTop = enemy.getY() - enemy.getSize() / 2.0f;
        float enemyBottom = enemy.getY() + enemy.getSize() / 2.0f;

        // Find the closest point to the bullet within the enemy.
        float closestX = Clamp(x, enemyLeft, enemyRight);
        float closestY = Clamp(y, enemyTop, enemyBottom);

        // Calculate the distance between the bullet's center and this closest point.
        float distanceX = x - closestX;
        float distanceY = y - closestY;

        // If the distance is less than the bullet's radius, an intersection occurs.
        float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);

        // Make bullet collider slightly larger than bullet's size for player convenience. :)
        float radius = size / 1.65f;

        return distanceSquared < (radius * radius);
    }
};
