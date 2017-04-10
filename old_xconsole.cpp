/**
 * This file belongs to the 'xlab' game engine.
 * Copyright 2009 xfacter
 * Copyright 2016 wickles
 * This work is licensed under the LGPLv3
 * subject to all terms as reproduced in the included LICENSE file.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "xtexture.h"
#include "xtext.h"

#include "xconsole.h"

/* ConsoleString */

xConsoleString::xConsoleString(int max_string_len)
{
    remain = 0.0f;
    alpha = 0.0f;
    color = 0;
    string_length = (u16)max_string_len;
    string = (char*)malloc(string_length*sizeof(char));
    *this = "DEAD";
}

xConsoleString::~xConsoleString()
{
    free(string);
}

xConsoleString& xConsoleString::operator=(char* new_string)
{
    snprintf(string, string_length, new_string);
    return *this;
}

xConsoleString& xConsoleString::operator=(const xConsoleString& new_string)
{
    remain = new_string.remain;
    alpha = new_string.alpha;
    color = new_string.color;
    snprintf(string, string_length, new_string.string);
    return *this;
}

char* xConsoleString::String()
{
    return string;
}

bool xConsoleString::Alive()
{
    return (alpha > 0.0f);
}

void xConsoleString::Recharge()
{
    remain = LIFE_PRECISION;
    alpha = LIFE_PRECISION;
}

void xConsoleString::Update(float dt, float remain_fade, float alpha_fade)
{
    if (Alive())
    {
        if (remain > 0) remain -= remain_fade*dt;
        else alpha -= alpha_fade*dt;
    }
}

u8 xConsoleString::Alpha()
{
    if (!Alive()) return 0;
    return (u8)(255.0f*(alpha/LIFE_PRECISION));
}

unsigned int xConsoleString::Color()
{
    unsigned int col = color & 0x00ffffff;
    col = col | (Alpha() << 24);
    return col;
}

/* xConsole */

xConsole::xConsole(int max_strings, int max_string_len)
{
    num_strings = (u16)max_strings;
    string_length = (u16)max_string_len;

    console_strings = new xConsoleString*[num_strings];
    for (int i = 0; i < num_strings; i++) console_strings[i] = new xConsoleString(string_length);
}

xConsole::xConsole(int max_strings, int max_string_len, float remain_time, float fade_time)
{
    num_strings = (u16)max_strings;
    string_length = (u16)max_string_len;

    console_strings = new xConsoleString*[num_strings];
    for (int i = 0; i < num_strings; i++) console_strings[i] = new xConsoleString(string_length);

    Settings(remain_time, fade_time);
}

xConsole::~xConsole()
{
    for (int i = 0; i < num_strings; i++) delete console_strings[i];
    delete[] console_strings;
}

void xConsole::Settings(float remain_time, float fade_time)
{
    remain_speed = LIFE_PRECISION/remain_time;
    fade_speed = LIFE_PRECISION/fade_time;
}

void xConsole::Printf(unsigned int color, char* string, ... )
{
    char buffer[256];
    va_list ap;
    va_start(ap, string);
    vsnprintf(buffer, sizeof(buffer), string, ap);
    va_end(ap);

    Print(color, buffer);
}

void xConsole::Print(unsigned int color, char* string)
{
    for (int i = num_strings - 1; i > 0; i--) *console_strings[i] = *console_strings[i-1];
    console_strings[0]->color = color;
    *console_strings[0] = string;
    console_strings[0]->Recharge();
}

void xConsole::Refresh()
{
    for (int i = 0; i < num_strings; i++) console_strings[i]->Recharge();
}

void xConsole::UpdateConsole(float dt)
{
    for (int i = 0; i < num_strings; i++) console_strings[i]->Update(dt, remain_speed, fade_speed);
}

void xConsole::RenderConsole(xTexture* font, int x, int y, int width, int height, int line_height, bool aa)
{
    if (!font || width < 0 || height <= 0 || line_height < 0) return;
    int line = y + height - line_height;
    if (width)
    {
        char buffer[256];
        char* str_begin;
        char* str_track;
        int str_len;
        u16 line_length[16];
        int lines;
        for (int i = 0; i < num_strings; i++)
        {
            if (line < y || !console_strings[i]->Alive()) break;

            str_begin = console_strings[i]->String();
            str_track = str_begin;
            str_len = strlen(str_begin);

            if (str_len > 0)
            {
                for (lines = 0; lines < 16; lines++)
                {
                    line_length[lines] = xText2DStringLength(font, 0, 0, 1.0f, width, buffer, str_track, sizeof(buffer));
                    str_track += line_length[lines];
                    if (line_length[lines] == 0 || (u32)str_track >= (u32)str_begin + str_len) break;
                }

                while(lines > 0 && line >= y)
                {
                    lines -= 1;
                    str_track -= line_length[lines];
                    xText2DStringLength(font, 0, 0, 1.0f, width, buffer, str_track, sizeof(buffer));
                    xText2DPrint(font, x, line, console_strings[i]->Color(), 0, 1.0f, aa, 0, buffer);
                    line -= line_height;
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < num_strings; i++)
        {
            if (line < y || !console_strings[i]->Alive()) break;
            xText2DPrint(font, x, line, console_strings[i]->Color(), 0, 1.0f, aa, 0, console_strings[i]->String());
            line -= line_height;
        }
    }
}
