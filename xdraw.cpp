#include <pspgu.h>
#include <pspgum.h>
#include "xgraphics.h"
#include "xmath.h"

#include "xdraw.h"

void xDrawPixel(int x, int y, u32 color)
{
    Vertex2D* pixel = (Vertex2D*)sceGuGetMemory(sizeof(Vertex2D));
    pixel[0].x = x;
    pixel[0].y = y;
    pixel[0].z = 0;
    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuColor(color);
    sceGuDrawArray(GU_POINTS, Vertex2D_vtype|GU_TRANSFORM_2D, 1, 0, pixel);
    xGuLoadStates();
}

void xDrawLine(int x1, int y1, int x2, int y2, int color)
{
    Vertex2D* line = (Vertex2D*)sceGuGetMemory(2*sizeof(Vertex2D));
    line[0].x = x1;
    line[0].y = y1;
    line[0].z = 0;
    line[1].x = x2;
    line[1].y = y2;
    line[1].z = 0;
    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuColor(color);
    sceGuDrawArray(GU_LINES, Vertex2D_vtype|GU_TRANSFORM_2D, 2, 0, line);
    xGuLoadStates();
}

void xDrawPoly(int x, int y, int sides, float radius, u32 color_center, u32 color, float rot_radians, bool lines)
{
    if (sides <= 2) return;
    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_DEPTH_TEST);
    
    float angle_increment = (2*PI)/sides;
    rot_radians *= -1;
    if (lines)
    {
        Vertex2D* lines = (Vertex2D*)sceGuGetMemory((sides+1)*sizeof(Vertex2D));
        int i;
        for (i = 0; i < sides+1; i++)
        {
            lines[i].x = (s16)(x + radius*x_cosf(rot_radians));
            lines[i].y = (s16)(y + radius*x_sinf(rot_radians));
            lines[i].z = 0;
            rot_radians += angle_increment;
        }
        sceGuColor(color);
        sceGuDrawArray(GU_LINE_STRIP, Vertex2D_vtype|GU_TRANSFORM_2D, sides+1, 0, lines);
    }
    else
    {
        CVertex2D* poly = (CVertex2D*)sceGuGetMemory((sides+2)*sizeof(CVertex2D));
        poly[0].color = color_center;
        poly[0].x = x;
        poly[0].y = y;
        poly[0].z = 0;
        int i;
        for (i = 1; i < sides+2; i++)
        {
            poly[i].color = color;
            poly[i].x = (s16)(x + radius*x_cosf(rot_radians));
            poly[i].y = (s16)(y + radius*x_sinf(rot_radians));
            poly[i].z = 0;
            rot_radians += angle_increment;
        }
        sceGuDrawArray(GU_TRIANGLE_FAN, CVertex2D_vtype|GU_TRANSFORM_2D, sides+2, 0, poly);
    }
    
    xGuLoadStates();
}

void xDrawCircle(int x, int y, float radius, int verts_per_quadrant, u32 color_center, u32 color, bool lines)
{
    if (verts_per_quadrant < 0) return;
    xDrawPoly(x, y, 4*verts_per_quadrant + 1, radius, color_center, color, 0, lines);
}

void xDrawQuad(int x1, int y1, u32 color1,
              int x2, int y2, u32 color2,
              int x3, int y3, u32 color3,
              int x4, int y4, u32 color4)
{
    CVertex2D* quad = (CVertex2D*)sceGuGetMemory(4*sizeof(CVertex2D));
    quad[0].color = color1;
    quad[0].x = x1;
    quad[0].y = y1;
    quad[0].z = 0;
    quad[1].color = color2;
    quad[1].x = x2;
    quad[1].y = y2;
    quad[1].z = 0;
    quad[2].color = color3;
    quad[2].x = x3;
    quad[2].y = y3;
    quad[2].z = 0;
    quad[3].color = color4;
    quad[3].x = x4;
    quad[3].y = y4;
    quad[3].z = 0;
    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuDrawArray(GU_TRIANGLE_FAN, CVertex2D_vtype|GU_TRANSFORM_2D, 4, 0, quad);
    xGuLoadStates();
}

void xDrawRectCorners(int x, int y, int width, int height, u32 color1, u32 color2, u32 color3, u32 color4)
{
    xDrawQuad(x, y, color1, x + width, y, color2, x + width, y + height, color3, x, y + height, color4);
}

void xDrawRectSides(int x, int y, int width, int height, u32 color_left, u32 color_right)
{
    xDrawRectCorners(x, y, width, height, color_left, color_right, color_right, color_left);
}

void xDrawRectSolid(int x, int y, int width, int height, u32 color)
{
    xDrawRectCorners(x, y, width, height, color, color, color, color);
}

void xDrawScreen(u32 color)
{
    xDrawRectSolid(0, 0, X_SCREEN_WIDTH, X_SCREEN_HEIGHT, color);
}

void xDraw3DRect(float x, float y, float z, float length, float height, u32 color)
{
    ScePspFMatrix4 view_mat;
    ScePspFVector3 translate;
    ScePspFVector3 pos = {x, y, z};
    
    sceGumMatrixMode(GU_VIEW);
    sceGumStoreMatrix(&view_mat);
    sceGumLoadIdentity();
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
    
    x_billboard(&translate, pos, &view_mat);
    
    sceGumLoadIdentity();
    sceGumTranslate(&translate);
    xGumScaleMulti(length, height, 0.0f);
    sceGuColor(color);
    
    xGuBlendDefault();
    
    xGuSaveStates();
    sceGuDisable(GU_TEXTURE_2D);
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_LIGHTING);
    
    sceGumDrawArray(xGenericVert_prim, xGenericVert_vtype|GU_TRANSFORM_3D, xGenericVert_count, 0, xGenericVert);
    
    xGuLoadStates();
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadMatrix(&view_mat);
    sceGumMatrixMode(GU_MODEL);
}

void xDrawTextureSection(int sx, int sy, int width, int height, xTexture* texture, int x, int y)
{
    texture->SetImage(0, 0);
    sceGuTexFunc(GU_TFX_REPLACE,GU_TCC_RGBA);
    xGuTexFilter(0);
    
    TVertex2D* vert = (TVertex2D*)sceGuGetMemory(2*sizeof(TVertex2D));
    vert[0].u = sx;
    vert[0].v = sy + height;
    vert[0].x = x;
    vert[0].y = y;
    vert[0].z = 0;
    vert[1].u = sx + width;
    vert[1].v = sy;
    vert[1].x = x + width;
    vert[1].y = y + height;
    vert[1].z = 0;
    
    xGuBlendDefault();
    
    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuEnable(GU_TEXTURE_2D);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuDrawArray(GU_SPRITES, TVertex2D_vtype|GU_TRANSFORM_2D, 2, 0, vert);
    xGuLoadStates();
}

void xDrawTexture(int x, int y, xTexture* texture)
{
    xDrawTextureSection(0, 0, texture->Width(), texture->Height(), texture, x, y);
}

void xDrawSkybox(xTexture* skybox_x_low, xTexture* skybox_x_high, xTexture* skybox_y_low, xTexture* skybox_y_high, xTexture* skybox_z_low, xTexture* skybox_z_high, 
                bool aa, int subdiv, float x, float y, float z, float x_length, float y_length, float z_length)
{
    if (subdiv <= 0) return;
    
    xGuSaveStates();
    sceGuEnable(GU_TEXTURE_2D);
    sceGuDisable(GU_LIGHTING);
    
    sceGuTexFunc(GU_TFX_DECAL,GU_TCC_RGBA);
    xGuTexFilter(aa);
    
    xGumSetPos(x, y, z);
    
    if (skybox_x_low)
    {
        skybox_x_low->SetImage(0, 0);
        int x;
        for (x = 0; x < subdiv; x++)
        {
            int y;
            for (y = 0; y < subdiv; y++)
            {
                TVertexF* verts = (TVertexF*)sceGuGetMemory(4*sizeof(TVertexF));
                verts[0].u = (float)x/subdiv;
                verts[0].v = (float)(y+1)/subdiv;
                verts[0].x = 0;
                verts[0].y = verts[0].u*y_length;
                verts[0].z = verts[0].v*z_length;
                verts[1].u = (float)(x+1)/subdiv;
                verts[1].v = (float)(y+1)/subdiv;
                verts[1].x = 0;
                verts[1].y = verts[1].u*y_length;
                verts[1].z = verts[1].v*z_length;
                verts[2].u = (float)(x+1)/subdiv;
                verts[2].v = (float)y/subdiv;
                verts[2].x = 0;
                verts[2].y = verts[2].u*y_length;
                verts[2].z = verts[2].v*z_length;
                verts[3].u = (float)x/subdiv;
                verts[3].v = (float)y/subdiv;
                verts[3].x = 0;
                verts[3].y = verts[3].u*y_length;
                verts[3].z = verts[3].v*z_length;
                sceGumDrawArray(GU_TRIANGLE_FAN, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D, 4, 0, verts);
            }
        }
    }
    
    if (skybox_x_high)
    {
        skybox_x_high->SetImage(0, 0);
        int x;
        for (x = 0; x < subdiv; x++)
        {
            int y;
            for (y = 0; y < subdiv; y++)
            {
                TVertexF* verts = (TVertexF*)sceGuGetMemory(4*sizeof(TVertexF));
                verts[0].u = (float)x/subdiv;
                verts[0].v = (float)(y+1)/subdiv;
                verts[0].x = x_length;
                verts[0].y = (1.0f-verts[0].u)*y_length;
                verts[0].z = verts[0].v*z_length;
                verts[1].u = (float)(x+1)/subdiv;
                verts[1].v = (float)(y+1)/subdiv;
                verts[1].x = x_length;
                verts[1].y = (1.0f-verts[1].u)*y_length;
                verts[1].z = verts[1].v*z_length;
                verts[2].u = (float)(x+1)/subdiv;
                verts[2].v = (float)y/subdiv;
                verts[2].x = x_length;
                verts[2].y = (1.0f-verts[2].u)*y_length;
                verts[2].z = verts[2].v*z_length;
                verts[3].u = (float)x/subdiv;
                verts[3].v = (float)y/subdiv;
                verts[3].x = x_length;
                verts[3].y = (1.0f-verts[3].u)*y_length;
                verts[3].z = verts[3].v*z_length;
                sceGumDrawArray(GU_TRIANGLE_FAN, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D, 4, 0, verts);
            }
        }
    }
    
    if (skybox_y_low)
    {
        skybox_y_low->SetImage(0, 0);
        int x;
        for (x = 0; x < subdiv; x++)
        {
            int y;
            for (y = 0; y < subdiv; y++)
            {
                TVertexF* verts = (TVertexF*)sceGuGetMemory(4*sizeof(TVertexF));
                verts[0].u = (float)x/subdiv;
                verts[0].v = (float)(y+1)/subdiv;
                verts[0].x = (1.0f-verts[0].u)*x_length;
                verts[0].y = 0;
                verts[0].z = verts[0].v*z_length;
                verts[1].u = (float)(x+1)/subdiv;
                verts[1].v = (float)(y+1)/subdiv;
                verts[1].x = (1.0f-verts[1].u)*x_length;
                verts[1].y = 0;
                verts[1].z = verts[1].v*z_length;
                verts[2].u = (float)(x+1)/subdiv;
                verts[2].v = (float)y/subdiv;
                verts[2].x = (1.0f-verts[2].u)*x_length;
                verts[2].y = 0;
                verts[2].z = verts[2].v*z_length;
                verts[3].u = (float)x/subdiv;
                verts[3].v = (float)y/subdiv;
                verts[3].x = (1.0f-verts[3].u)*x_length;
                verts[3].y = 0;
                verts[3].z = verts[3].v*z_length;
                sceGumDrawArray(GU_TRIANGLE_FAN, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D, 4, 0, verts);
            }
        }
    }
    
    if (skybox_y_high)
    {
        skybox_y_high->SetImage(0, 0);
        int x;
        for (x = 0; x < subdiv; x++)
        {
            int y;
            for (y = 0; y < subdiv; y++)
            {
                TVertexF* verts = (TVertexF*)sceGuGetMemory(4*sizeof(TVertexF));
                verts[0].u = (float)x/subdiv;
                verts[0].v = (float)(y+1)/subdiv;
                verts[0].x = verts[0].u*x_length;
                verts[0].y = y_length;
                verts[0].z = verts[0].v*z_length;
                verts[1].u = (float)(x+1)/subdiv;
                verts[1].v = (float)(y+1)/subdiv;
                verts[1].x = verts[1].u*x_length;
                verts[1].y = y_length;
                verts[1].z = verts[1].v*z_length;
                verts[2].u = (float)(x+1)/subdiv;
                verts[2].v = (float)y/subdiv;
                verts[2].x = verts[2].u*x_length;
                verts[2].y = y_length;
                verts[2].z = verts[2].v*z_length;
                verts[3].u = (float)x/subdiv;
                verts[3].v = (float)y/subdiv;
                verts[3].x = verts[3].u*x_length;
                verts[3].y = y_length;
                verts[3].z = verts[3].v*z_length;
                sceGumDrawArray(GU_TRIANGLE_FAN, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D, 4, 0, verts);
            }
        }
    }
    
    if (skybox_z_low)
    {
        skybox_z_low->SetImage(0, 0);
        int x;
        for (x = 0; x < subdiv; x++)
        {
            int y;
            for (y = 0; y < subdiv; y++)
            {
                TVertexF* verts = (TVertexF*)sceGuGetMemory(4*sizeof(TVertexF));
                verts[0].u = (float)x/subdiv;
                verts[0].v = (float)(y+1)/subdiv;
                verts[0].x = verts[0].u*x_length;
                verts[0].y = verts[0].v*y_length;
                verts[0].z = 0;
                verts[1].u = (float)(x+1)/subdiv;
                verts[1].v = (float)(y+1)/subdiv;
                verts[1].x = verts[1].u*x_length;
                verts[1].y = verts[1].v*y_length;
                verts[1].z = 0;
                verts[2].u = (float)(x+1)/subdiv;
                verts[2].v = (float)y/subdiv;
                verts[2].x = verts[2].u*x_length;
                verts[2].y = verts[2].v*y_length;
                verts[2].z = 0;
                verts[3].u = (float)x/subdiv;
                verts[3].v = (float)y/subdiv;
                verts[3].x = verts[3].u*x_length;
                verts[3].y = verts[3].v*y_length;
                verts[3].z = 0;
                sceGumDrawArray(GU_TRIANGLE_FAN, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D, 4, 0, verts);
            }
        }
    }
    
    if (skybox_z_high)
    {
        skybox_z_high->SetImage(0, 0);
        int x;
        for (x = 0; x < subdiv; x++)
        {
            int y;
            for (y = 0; y < subdiv; y++)
            {
                TVertexF* verts = (TVertexF*)sceGuGetMemory(4*sizeof(TVertexF));
                verts[0].u = (float)x/subdiv;
                verts[0].v = (float)(y+1)/subdiv;
                verts[0].x = verts[0].u*x_length;
                verts[0].y = (1.0f-verts[0].v)*y_length;
                verts[0].z = z_length;
                verts[1].u = (float)(x+1)/subdiv;
                verts[1].v = (float)(y+1)/subdiv;
                verts[1].x = verts[1].u*x_length;
                verts[1].y = (1.0f-verts[1].v)*y_length;
                verts[1].z = z_length;
                verts[2].u = (float)(x+1)/subdiv;
                verts[2].v = (float)y/subdiv;
                verts[2].x = verts[2].u*x_length;
                verts[2].y = (1.0f-verts[2].v)*y_length;
                verts[2].z = z_length;
                verts[3].u = (float)x/subdiv;
                verts[3].v = (float)y/subdiv;
                verts[3].x = verts[3].u*x_length;
                verts[3].y = (1.0f-verts[3].v)*y_length;
                verts[3].z = z_length;
                sceGumDrawArray(GU_TRIANGLE_FAN, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D, 4, 0, verts);
            }
        }
    }
    
    xGuLoadStates();
}
