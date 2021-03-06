#pragma once

#include <cstdint>
#include "../lib/leetlib.h"
#include "player.h"

namespace Invaders
{

// Enemy class represents an enemy. Handles movement, status, AI and collisions between enemy and player.
// Enemy collider is implemented as a rectangle.
class Enemy
{
public:
    Enemy() :
        x(0.0f),
        y(0.0f),
        xo(0.0f),
        yo(0.0f),
        size(5.0f),
        sprite(nullptr),
        order(0),
        active(true),
        enraged(false),
        enragedSpeed(2.0f)
    {}

    void update(const uint32_t currentTime)
    {
        if (!active)
        {
            return;
        }

        if (intersectsPlayer())
        {
            player->destroy();
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

    void initialize(const float x, const float y, const float size, void* sprite, const int order, Player* player)
    {
        this->x = x;
        this->y = y;
        this->size = size;
        this->sprite = sprite;
        this->order = order;
        this->player = player;
    }

    void destroy()
    {
        active = false;
    }

    void enrage(const float enragedSpeed)
    {
        x = getX();
        y = getY();
        xo = 0.0f;
        yo = 0.0f;

        this->enragedSpeed = enragedSpeed;
        this->enraged = true;
    }

    float getX() const
    {
        return x + xo;
    }

    float getY() const
    {
        return y + yo;
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

    bool isEnraged() const
    {
        return enraged;
    }

    bool isTransitioning() const
    {
        float distanceFromSpawn = sqrtf(xo * xo + yo * yo);
        return distanceFromSpawn > 20.0f;
    }

private:
    float x;
    float y;
    float xo;
    float yo;
    float size;
    void* sprite;
    int order;
    bool active;
    bool enraged;
    float enragedSpeed;
    Player* player;

    void magicAI(const uint32_t currentTime)
    {
        xo = 0.0f;
        yo = 0.0f;
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

        DrawSprite(sprite, getX(), getY(), float((10 + (order % 17))), float((10 + (order % 17))), 0.0f, 0xffffffff);
    }

    void enragedAI()
    {
        // Move closer to player
        if (player->isActive())
        {
            float u = player->getX() - x;
            float v = player->getY() - y;

            // Normalize movement vector
            float length = sqrtf(u * u + v * v);
            if (length != 0.0f)
            {
                u /= length;
                v /= length;
            }

            x += u * enragedSpeed;
            y += v * enragedSpeed;
        }

        DrawSprite(sprite, x, y, float((10 + (order % 17))), float((10 + (order % 17))), 0.0f, 0xffff0000);
    }

    // Checks for collision between an enemy and a player. Both enemy and player colliders are implemented as rectangles.
    bool intersectsPlayer() const
    {
        float enemyLeft = getX() - size / 2.0f;
        float enemyRight = getX() + size / 2.0f;
        float enemyTop = getY() - size / 2.0f;
        float enemyBottom = getY() + size / 2.0f;

        float playerLeft = player->getX() - player->getSize() / 2.0f;
        float playerRight = player->getX() + player->getSize() / 2.0f;
        float playerTop = player->getY() - player->getSize() / 2.0f;
        float playerBottom = player->getY() + player->getSize() / 2.0f;

        if (enemyLeft < playerRight && enemyRight > playerLeft && enemyTop < playerBottom && enemyBottom > playerTop)
        {
            return true;
        }

        return false;
    }
};

} // namespace Invaders
