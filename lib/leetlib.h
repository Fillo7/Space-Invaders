#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <cstdint>
#include <string>
#include <windows.h>

//*************************************************************************************************************************************************//
// Constants and utility functions.

// Duration of one second in frames.
const uint32_t SECOND_DURATION = 60;

// Value of Pi.
const float PI = 3.1415926535f;

// Returns min if value is less than min, returns max if max is less than value, otherwise returns v. Max must be greater than min.
template <typename T> const T& Clamp(const T& value, const T& min, const T& max)
{
    if (max < min)
    {
        return value;
    }
    return value < min ? min : max < value ? max : value;
}

//*************************************************************************************************************************************************//
// Core functions

// WantQuit returns true if game should be terminated.
bool WantQuit(DWORD clearColor = 0);

// Flips the screen, frame locked to 60 hz. Screen is always 800 x 600.
void Flip(); 

// Main game loop.
void Game();

//*************************************************************************************************************************************************//
// Input

// Returns mouse position, (0, 0) is top left, (800, 600) is bottom right.
void GetMousePosition(float& x, float& y); 

// Use VK_ codes for special keys, capital chars for letter keys. Use VK_LBUTTON and VK_RBUTTON for mouse buttons.
bool IsKeyDown(int key); 

//*************************************************************************************************************************************************//
// Sprites drawing

// Loads sprite from file.
void* LoadSprite(const std::string& filename);

// Draws previously loaded sprite.
void DrawSprite(void* sprite, float xCentre, float yCentre, float xSize, float ySize, float rotationAngleRadians = 0.0f, DWORD tintColorARGB = 0xffffffff);

//*************************************************************************************************************************************************//
// Sound

// Loads music from file. Returns a handle which can be used to change volume or stop sound.
int PlayMusic(const std::string& filename, float volume = 1);

// Stops music.
void StopMusic();

// Loads sound from file. If loop is set, the sample will loop forever until StopSnd() is called.
void* LoadSnd(const std::string& filename, bool loop = false);

// Plays specified sound. Returns a handle which can be used to change volume or stop sound.
int PlaySnd(void* sound, float volume = 1.0f);

// Stops sound corresponding to specified handle.
void StopSnd(int handle);

// Changes volume of sound corresponding to specified handle.
void ChangeVolume(int handle, float newVolume = 1.0f);
