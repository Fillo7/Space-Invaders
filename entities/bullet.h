#pragma once

#include <cstdint>
#include "../lib/leetlib.h"

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

    bool intersectsEnemy()
    {
        // clamp(value, min, max) - limits value to the range min..max

        // Find the closest point to the circle within the rectangle
        /*float closestX = clamp(circle.X, rectangle.Left, rectangle.Right);
        float closestY = clamp(circle.Y, rectangle.Top, rectangle.Bottom);

        // Calculate the distance between the circle's center and this closest point
        float distanceX = circle.X - closestX;
        float distanceY = circle.Y - closestY;

        // If the distance is less than the circle's radius, an intersection occurs
        float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);
        return distanceSquared < (circle.Radius * circle.Radius);*/

        return false;
    }
};
