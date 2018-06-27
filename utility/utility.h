#pragma once

#include <array>
#include <cstdint>
#include <map>
#include <string>
#include "../entities/enemy.h"

namespace Invaders
{

// Loads all sprites and returns map that can be used to access them.
std::map<std::string, void*> getSpriteMap();

// Draws specified text on screen based on provided configuration options.
void drawText(const std::string& text, const std::map<std::string, void*>& spriteMap, const uint32_t time, const int gap, const float size,
    const float xOffset, const float yOffset, const bool enableRotation);

// Saves high score to specified file.
void saveHighScore(const std::string& filename, const uint32_t highScore);

// Loads and returns saved high score from specified file.
uint32_t loadHighScore(const std::string& filename);

// Calculates score based on defeated enemies count and optionally based on current time.
uint32_t getCurrentScore(const std::array<Enemy, 50>& enemies, const uint32_t time, const bool includeTimeBonus);

// Calculates bonus score based on current time.
uint32_t getBonusScore(const uint32_t time);

} // namespace Invaders
