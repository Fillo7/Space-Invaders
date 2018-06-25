#include <iostream>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <list>
#include "lib/leetlib.h"

class Timer
{
public:
    Timer() :
        initialTime(std::chrono::steady_clock::now())
    {}
    
    uint64_t getElapsedTimeInMilliseconds()
    {
        endTime = std::chrono::steady_clock::now();
        uint64_t result = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - initialTime).count());
        initialTime = endTime;
        return result;
    }

private:
    std::chrono::steady_clock::time_point initialTime;
    std::chrono::steady_clock::time_point endTime;
};

class PlayerShip
{
public:
    PlayerShip() :
        PlayerShip(nullptr)
    {}

    PlayerShip(void* sprite) :
        x(400.0f),
        y(550.0f),
        size(50.0f),
        sprite(sprite)
    {}

    void update(const uint64_t time)
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
        DrawSprite(sprite, x, y, size, size, PI + sin(time * 0.1f) * 0.1f, 0xffffffff);
    }

    float getX() const
    {
        return x;
    }

    float getY() const
    {
        return y;
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
        Bullet(0.0f, 0.0f, nullptr)
    {}

    Bullet(const float x, const float y, void* sprite) :
        x(x),
        y(y),
        angle(0.0f),
        timeToLive(8'000),
        sprite(sprite)
    {}

    void update(const int deltaTime)
    {
        timeToLive -= deltaTime;
        y -= 4.0f;
        angle += 0.1f;
    }

    bool isExpired() const
    {
        return timeToLive <= 0;
    }

    float getX() const
    {
        return x;
    }

    float getY() const
    {
        return y;
    }

    float getAngle() const
    {
        return angle;
    }

    void* getSprite()
    {
        return sprite;
    }

public:
    float x;
    float y;
    float angle;
    int timeToLive;
    void* sprite;
};

int x[50];
int y[50];
std::list<Bullet> activeBullets;

void updateBullets(const uint64_t deltaTime)
{
    for (auto iterator = activeBullets.begin(); iterator != activeBullets.end();)
    {
        iterator->update(int(deltaTime));

        if (iterator->isExpired())
        {
            iterator = activeBullets.erase(iterator);
        }
        else
        {
            DrawSprite(iterator->getSprite(), iterator->getX(), iterator->getY(), 10.0f, 10.0f, iterator->getAngle(), 0xffffffff);
            ++iterator;
        }
    }
}

void Game()
{
	void *Text[]=
	{
		LoadSprite("gfx/slet.png"),
		LoadSprite("gfx/plet.png"),
		LoadSprite("gfx/alet.png"),
		LoadSprite("gfx/clet.png"),
		LoadSprite("gfx/elet.png"),
		0,
		LoadSprite("gfx/ilet.png"),
		LoadSprite("gfx/nlet.png"),
		LoadSprite("gfx/vlet.png"),
		LoadSprite("gfx/alet.png"),
		LoadSprite("gfx/dlet.png"),
		LoadSprite("gfx/elet.png"),
		LoadSprite("gfx/rlet.png"),
		LoadSprite("gfx/slet.png")
	};

	// SETUP
    uint64_t time = 0;
    uint64_t updateTimer = 0;
	void *Enemy = LoadSprite("gfx/Little Invader.png");
	void* playerSprite = LoadSprite("gfx/Big Invader.png");
	void* bulletSprite = LoadSprite("gfx/bullet.png");
	for(int n=0;n<50;++n)
	{
		x[n]=(n%10)*60+120;
		y[n]=(n/10)*60+70;
	}

    PlayerShip player(playerSprite);
    Timer timer;
    while (!WantQuit() && !IsKeyDown(VK_ESCAPE))
    {
        time++;
        updateTimer += timer.getElapsedTimeInMilliseconds();

	    for (int n = 0; n < 50; ++n)
	    {
            int xo = 0;
            int yo = 0;
		    int n1 = int(time) + n*n + n*n*n;
		    int n2 = int(time) + n+ n*n + 3*n*n*n;
		    if(((n1>>6)&0x7)==0x7)xo+=(1-cos((n1&0x7f)/64.0f*2.f*PI))*(20+((n*n)%9));
		    if(((n1>>6)&0x7)==0x7)yo+=(sin((n1&0x7f)/64.0f*2.f*PI))*(20+((n*n)%9));
		    if(((n2>>8)&0xf)==0xf)yo+=(1-cos((n2&0xff)/256.0f*2.f*PI))*(150+((n*n)%9));
		    DrawSprite(Enemy, (float)(x[n]+xo), (float)(y[n]+yo), (float)(10+((n)%17)), (float)(10+((n)%17)), 0, 0xffffffff);
	    }
	    
        player.update(time);

	    // FIRE
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
            activeBullets.push_back(Bullet(player.getX(), player.getY(), bulletSprite));
            bulletCooldown = 15;
        }

        updateBullets(updateTimer);
        updateTimer = 0;

        for (int n = 0; n < (int)strlen("space invaders"); ++n)
        {
            if (n != 5)
            {
                DrawSprite(Text[n], float(n * 40 + 150), 30.0f, 20.0f, 20.0f, float((sin(time * 0.1f) * n * 0.01f)));
            }
        }

	    Flip();
    }
    
    return;
}

void OldGame()
{
	void *sprite = LoadSprite("sprite.png");
	float size=10;
	float angle=0;
	while (!WantQuit() && !IsKeyDown(VK_ESCAPE))
	{
		angle+=0.01f;
		float mx,my;
		GetMousePos(mx,my);
		DWORD col=0xffffffff; // white
		if (IsKeyDown(VK_LBUTTON)) col=0xffff0000; // solid red
		if (IsKeyDown(VK_RBUTTON)) col=0x8000ff00; // 50% transparent green
		if (IsKeyDown(VK_UP)) size++;
		if (IsKeyDown(VK_DOWN)) size--;
		
		DrawSprite(sprite,400,300,100,100, angle);
		DrawSprite(sprite, mx,my, size,size, 0, col);	
		Flip();
	}
}
