/**
 * This file belongs to the 'xlab' game engine.
 * Copyright 2009 xfacter
 * Copyright 2016 wickles
 * This work is licensed under the LGPLv3
 * subject to all terms as reproduced in the included LICENSE file.
 */

#ifndef __X_SCREEN_H__
#define __X_SCREEN_H__

#include "xconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define X_SCREEN_MULTIPLY (0)
#define X_SCREEN_ADD      (1)
#define X_SCREEN_SUBTRACT (2)
#define X_SCREEN_REVERSE_SUBTRACT (3)

/* WHOWHOAHAOWHA ! flatmush FS effects dont seem to work with double display lists! whats up ? */

/* extremely slow, real-time use not recommended. very fun to use though! see ops above */
//void xScreenModify(int op, u8 red, u8 green, u8 blue);

/* move these to xbuffer.h? */

/* works, but could be optimized. possibly render draw buffer at smaller size for quicker rendering */
/* note: resets tex filter to no filter */
void xScreenRadialBlur(int num, float scale_inc, float rot_inc, float alpha_start);

void xScreenSimpleBlur(float alpha);

void xScreenGaussianBlur(float radius);

/* some kind of motion blur would be good */

/* cleanup function for when finished with palettes (created when functions are called for first time) */
void xScreenDestroyPalettes();

void xScreenGrayscale();

void xScreenNight();

void xScreenThermal();

void xScreenInverted();

void xScreenMonochrome(u32 color);

void xScreenDichrome(u32 light, u32 dark);

void xScreenBinary(u32 light, u32 dark, u8 threshold);

void xScreenColorReduce(u8 divisions);

void xScreenColorEnhance();

void xScreenLightEnhance();

void xScreenDepthFog(u32 color, int lowbit);

#ifdef __cplusplus
}
#endif

#endif
