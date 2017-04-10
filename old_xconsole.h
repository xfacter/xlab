/**
 * This file belongs to the 'xlab' game engine.
 * Copyright 2009 xfacter
 * Copyright 2016 wickles
 * This work is licensed under the LGPLv3
 * subject to all terms as reproduced in the included LICENSE file.
 */

#pragma once

#include <psptypes.h>
#include "xtexture.h"

#define LIFE_PRECISION (1000.0f)

class xConsoleString
{
private:
    float remain;
    float alpha;
    u16 string_length;
    char* string;
public:
    unsigned int color;

    xConsoleString(int max_string_len);
    ~xConsoleString();
    xConsoleString& operator=(char* new_string);
    xConsoleString& operator=(const xConsoleString& new_string);
    char* String();
    bool Alive();
    void Recharge();
    void Update(float dt, float remain_fade, float alpha_fade);
    u8 Alpha();
    unsigned int Color();
};

class xConsole
{
private:
    u16 num_strings;
    u16 string_length;
    float remain_speed;
    float fade_speed;
    xConsoleString** console_strings;
public:
    xConsole(int max_strings, int max_string_len);
    xConsole(int max_strings, int max_string_len, float remain_time, float fade_time);
    ~xConsole();
    void Settings(float remain_time, float fade_time);
    void Printf(unsigned int color, char* string, ... );
    void Print(unsigned int color, char* string);
    void Refresh();
    void UpdateConsole(float dt);
    void RenderConsole(xTexture* font, int x, int y, int width, int height, int line_height, bool aa);
};
