#ifndef __X_DRAW_H__
#define __X_DRAW_H__

#include <psptypes.h>
#include "xtexture.h"

/* Draw */

void xDrawPixel(int x, int y, u32 color);

void xDrawLine(int x1, int y1, int x2, int y2, int color);

void xDrawPoly(int x, int y, int sides, float radius, u32 color_center, u32 color, float rot_radians, bool lines);

void xDrawCircle(int x, int y, float radius, int verts_per_quadrant, u32 color_center, u32 color, bool lines);

void xDrawQuad(int x1, int y1, u32 color1, int x2, int y2, u32 color2, int x3, int y3, u32 color3, int x4, int y4, u32 color4);

void xDrawRectCorners(int x, int y, int width, int height, u32 color1, u32 color2, u32 color3, u32 color4);

void xDrawRectSides(int x, int y, int width, int height, u32 color_left, u32 color_right);

void xDrawRectSolid(int x, int y, int width, int height, u32 color);

void xDrawScreen(u32 color);

void xDraw3DRect(float x, float y, float z, float length, float height, u32 color);

void xDrawTextureSection(int sx, int sy, int width, int height, xTexture* texture, int x, int y);

void xDrawTexture(int x, int y, xTexture* texture);

void xDrawSkybox(xTexture* skybox_x_low, xTexture* skybox_x_high, xTexture* skybox_y_low, xTexture* skybox_y_high, xTexture* skybox_z_low, xTexture* skybox_z_high, 
                 bool aa, int subdiv, float x, float y, float z, float x_length, float y_length, float z_length);

#endif
