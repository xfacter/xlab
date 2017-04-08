#include <pspgu.h>
#include <pspgum.h>
#include "xgraphics.h"
#include "xmath.h"

#include "xdraw.h"

void xDrawPoint(int x, int y, u32 c)
{
    Vertex2D* vertices = (Vertex2D*)sceGuGetMemory(sizeof(Vertex2D));
    vertices[0].x = x;
    vertices[0].y = y;
    vertices[0].z = 0;
    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuDisable(GU_TEXTURE_2D);
    sceGuColor(c);
    sceGuDrawArray(GU_POINTS, Vertex2D_vtype|GU_TRANSFORM_2D, 1, 0, vertices);
    xGuLoadStates();
}

void xDrawLine(int x0, int y0, u32 c0, int x1, int y1, u32 c1)
{
    CVertex2D* vertices = (CVertex2D*)sceGuGetMemory(2*sizeof(CVertex2D));
    vertices[0].color = c0;
    vertices[0].x = x0;
    vertices[0].y = y0;
    vertices[0].z = 0;
    vertices[1].color = c1;
    vertices[1].x = x1;
    vertices[1].y = y1;
    vertices[1].z = 0;
    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuDisable(GU_TEXTURE_2D);
    sceGuDrawArray(GU_LINES, CVertex2D_vtype|GU_TRANSFORM_2D, 2, 0, vertices);
    xGuLoadStates();
}

void xDrawTri3C(int x0, int y0, u32 c0, int x1, int y1, u32 c1, int x2, int y2, u32 c2)
{
    CVertex2D* vertices = (CVertex2D*)sceGuGetMemory(3*sizeof(CVertex2D));
    vertices[0].color = c0;
    vertices[0].x = x0;
    vertices[0].y = y0;
    vertices[0].z = 0;
    vertices[1].color = c1;
    vertices[1].x = x1;
    vertices[1].y = y1;
    vertices[1].z = 0;
    vertices[2].color = c2;
    vertices[2].x = x2;
    vertices[2].y = y2;
    vertices[2].z = 0;
    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuDisable(GU_TEXTURE_2D);
    sceGuDrawArray(GU_TRIANGLE_FAN, CVertex2D_vtype|GU_TRANSFORM_2D, 3, 0, vertices);
    xGuLoadStates();
}

void xDrawTri(int x0, int y0, int x1, int y1, int x2, int y2, u32 c)
{
    xDrawTri3C(x0, y0, c, x1, y1, c, x2, y2, c);
}

void xDrawTriOutline(int x0, int y0, int x1, int y1, int x2, int y2, u32 c)
{
    Vertex2D* vertices = (Vertex2D*)sceGuGetMemory(4*sizeof(Vertex2D));
    vertices[0].x = x0;
    vertices[0].y = y0;
    vertices[0].z = 0;
    vertices[1].x = x1;
    vertices[1].y = y1;
    vertices[1].z = 0;
    vertices[2].x = x2;
    vertices[2].y = y2;
    vertices[2].z = 0;
    vertices[3] = vertices[0];
    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuDisable(GU_TEXTURE_2D);
    sceGuColor(c);
    sceGuDrawArray(GU_LINES, Vertex2D_vtype|GU_TRANSFORM_2D, 4, 0, vertices);
    xGuLoadStates();
}

void xDrawRect4C(int x, int y, int w, int h, u32 c0, u32 c1, u32 c2, u32 c3)
{
    CVertex2D* vertices = (CVertex2D*)sceGuGetMemory(4*sizeof(CVertex2D));
    vertices[0].color = c0;
    vertices[0].x = x;
    vertices[0].y = y;
    vertices[0].z = 0;
    vertices[1].color = c1;
    vertices[1].x = x + w;
    vertices[1].y = y;
    vertices[1].z = 0;
    vertices[2].color = c2;
    vertices[2].x = x + w;
    vertices[2].y = y + h;
    vertices[2].z = 0;
    vertices[3].color = c3;
    vertices[3].x = x;
    vertices[3].y = y + h;
    vertices[3].z = 0;
    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuDisable(GU_TEXTURE_2D);
    sceGuDrawArray(GU_TRIANGLE_FAN, CVertex2D_vtype|GU_TRANSFORM_2D, 4, 0, vertices);
    xGuLoadStates();
}

void xDrawRect(int x, int y, int w, int h, u32 c)
{
    xDrawRect4C(x, y, w, h, c, c, c, c);
}

void xDrawRectOutline(int x, int y, int w, int h, u32 c)
{
    Vertex2D* vertices = (Vertex2D*)sceGuGetMemory(5*sizeof(Vertex2D));
    vertices[0].x = x;
    vertices[0].y = y;
    vertices[0].z = 0;
    vertices[1].x = x + w;
    vertices[1].y = y;
    vertices[1].z = 0;
    vertices[2].x = x + w;
    vertices[2].y = y + h;
    vertices[2].z = 0;
    vertices[3].x = x;
    vertices[3].y = y + h;
    vertices[3].z = 0;
    vertices[4] = vertices[0];
    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuDisable(GU_TEXTURE_2D);
    sceGuColor(c);
    sceGuDrawArray(GU_LINES, Vertex2D_vtype|GU_TRANSFORM_2D, 5, 0, vertices);
    xGuLoadStates();
}

void xDrawPolygon(int x, int y, float r, int n, u32 c0, u32 c1)
{
    float angle = X_PI_2;
    float angle_increment = X_2PI/n;
    CVertex2D* vertices = (CVertex2D*)sceGuGetMemory((n+2)*sizeof(CVertex2D));
    vertices[0].color = c0;
    vertices[0].x = 0;
    vertices[0].y = 0;
    vertices[0].z = 0;
    int i;
    for (i = 1; i < n+1; i++)
    {
        vertices[i].color = c1;
        vertices[i].x = (int)(r*x_cosf(angle));
        vertices[i].y = (int)(-r*x_sinf(angle));
        vertices[i].z = 0;
        angle += angle_increment;
    }
    vertices[n+1] = vertices[1];
    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_LIGHTING);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuDisable(GU_TEXTURE_2D);
    sceGuDrawArray(GU_TRIANGLE_FAN, CVertex2D_vtype|GU_TRANSFORM_2D, n+2, 0, vertices);
    xGuLoadStates();
}

void xDrawPolygonOutline(int x, int y, float r, int n, u32 c)
{
    float angle = X_PI_2;
    float angle_increment = X_2PI/n;
    Vertex2D* vertices = (Vertex2D*)sceGuGetMemory((n+1)*sizeof(Vertex2D));
    int i;
    for (i = 0; i < n; i++)
    {
        vertices[i].x = (int)(r*x_cosf(angle));
        vertices[i].y = (int)(-r*x_sinf(angle));
        vertices[i].z = 0;
        angle += angle_increment;
    }
    vertices[n] = vertices[0];
    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_LIGHTING);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuDisable(GU_TEXTURE_2D);
    sceGuDrawArray(GU_LINES, Vertex2D_vtype|GU_TRANSFORM_2D, n+1, 0, vertices);
    xGuLoadStates();
}

void xDrawPolygon2(int x, int y, float r0, float r1, int n, u32 c0, u32 c1, u32 c2)
{
    float angle = X_PI_2;
    float angle_increment = X_2PI/n;
    CVertex2D* vertices = (CVertex2D*)sceGuGetMemory((2*n+2)*sizeof(CVertex2D));
    vertices[0].color = c0;
    vertices[0].x = 0;
    vertices[0].y = 0;
    vertices[0].z = 0;
    int i;
    for (i = 1; i < n+1; i++)
    {
        vertices[2*i-1].color = c1;
        vertices[2*i-1].x = (int)(r0*x_cosf(angle));
        vertices[2*i-1].y = (int)(-r0*x_sinf(angle));
        vertices[2*i-1].z = 0;
        vertices[2*i-0].color = c2;
        vertices[2*i-0].x = (int)((r0+r1)*x_cosf(angle+0.5f*angle_increment));
        vertices[2*i-0].y = (int)(-(r0+r1)*x_sinf(angle+0.5f*angle_increment));
        vertices[2*i-0].z = 0;
        angle += angle_increment;
    }
    vertices[2*n+1] = vertices[1];
    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_LIGHTING);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuDisable(GU_TEXTURE_2D);
    sceGuDrawArray(GU_TRIANGLE_FAN, CVertex2D_vtype|GU_TRANSFORM_2D, 2*n+2, 0, vertices);
    xGuLoadStates();
}

void xDrawPolygon2Outline(int x, int y, float r0, float r1, int n, u32 c)
{
    float angle = X_PI_2;
    float angle_increment = X_2PI/n;
    Vertex2D* vertices = (Vertex2D*)sceGuGetMemory((2*n+1)*sizeof(Vertex2D));
    int i;
    for (i = 0; i < n; i++)
    {
        vertices[2*i+0].x = (int)(r0*x_cosf(angle));
        vertices[2*i+0].y = (int)(-r0*x_sinf(angle));
        vertices[2*i+0].z = 0;
        vertices[2*i+1].x = (int)((r0+r1)*x_cosf(angle+0.5f*angle_increment));
        vertices[2*i+1].y = (int)(-(r0+r1)*x_sinf(angle+0.5f*angle_increment));
        vertices[2*i+1].z = 0;
        angle += angle_increment;
    }
    vertices[2*n] = vertices[1];
    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_LIGHTING);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuDisable(GU_TEXTURE_2D);
    sceGuColor(c);
    sceGuDrawArray(GU_LINES, Vertex2D_vtype|GU_TRANSFORM_2D, 2*n+2, 0, vertices);
    xGuLoadStates();
}
