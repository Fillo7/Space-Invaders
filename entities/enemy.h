#pragma once

#include <cstdint>
#include "../lib/leetlib.h"
#include "player.h"

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
        enraged(false)
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
    float xo;
    float yo;
    float size;
    void* sprite;
    int order;
    bool active;
    bool enraged;
    Player* player;

    void magicAI(const uint32_t currentTime)
    {
        xo = 0;
        yo = 0;
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

    bool intersectsPlayer() const
    {
        float enemyLeft = (x + xo) - size / 2.0f;
        float enemyRight = (x + xo) + size / 2.0f;
        float enemyTop = (y + yo) - size / 2.0f;
        float enemyBottom = (y + yo) + size / 2.0f;

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
