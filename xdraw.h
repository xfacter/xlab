/**
 * This file belongs to the 'xlab' game engine.
 * Copyright 2009 xfacter
 * Copyright 2016 wickles
 * This work is licensed under the LGPLv3
 * subject to all terms as reproduced in the included LICENSE file.
 */

#ifndef __X_DRAW_H__
#define __X_DRAW_H__

#include "xconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

void xDrawPoint(int x, int y, u32 c);

void xDrawLine(int x0, int y0, u32 c0, int x1, int y1, u32 c1);

void xDrawTri3C(int x0, int y0, u32 c0, int x1, int y1, u32 c1, int x2, int y2, u32 c2);
void xDrawTri(int x0, int y0, int x1, int y1, int x2, int y2, u32 c);
void xDrawTriOutline(int x0, int y0, int x1, int y1, int x2, int y2, u32 c);

void xDrawRect4C(int x, int y, int w, int h, u32 c0, u32 c1, u32 c2, u32 c3);
void xDrawRect(int x, int y, int w, int h, u32 c);
void xDrawRectOutline(int x, int y, int w, int h, u32 c);

void xDrawPolygon(int x, int y, float r, int n, u32 c0, u32 c1);
void xDrawPolygonOutline(int x, int y, float r, int n, u32 c);

/* polygon2 has inner radius r0 that connects to triangles extending out further by r1 */
void xDrawPolygon2(int x, int y, float r0, float r1, int n, u32 c0, u32 c1, u32 c2);
void xDrawPolygon2Outline(int x, int y, float r0, float r1, int n, u32 c);

#ifdef __cplusplus
}
#endif

#endif
