/**
 * This file belongs to the 'xlab' game engine.
 * Copyright 2009 xfacter
 * Copyright 2016 wickles
 * This work is licensed under the LGPLv3
 * subject to all terms as reproduced in the included LICENSE file.
 */

/* some prims malloc memory to read calculated values into. we dont want to log every alloc/free, so use stdlib instead of xmem */
#include <stdlib.h>
#include <pspgu.h>
#include <pspgum.h>
#include "xgraphics.h"
#include "xmath.h"

#include "xprim.h"

inline int attr_vtype(int attr)
{
    return (GU_VERTEX_32BITF|(attr & X_PRIM_TEXTURE ? GU_TEXTURE_32BITF : 0)|(attr & X_PRIM_NORMAL ? GU_NORMAL_32BITF : 0));
}

inline int attr_vert_size(int attr)
{
    return (sizeof(float)*(3 + (attr & X_PRIM_TEXTURE ? 2 : 0) + (attr & X_PRIM_NORMAL ? 3 : 0)));
}

#define SET_VECTOR(V, X, Y, Z) V.x = (X); V.y = (Y); V.z = (Z)

#define SET_TEXTURE(ATTR, VERT, IND, U, V) \
    if (ATTR & X_PRIM_TEXTURE)             \
    {                                      \
        VERT[IND++] = U;                   \
        VERT[IND++] = V;                   \
    }

#define SET_NORMAL(ATTR, VERT, IND, X, Y, Z) \
    if (ATTR & X_PRIM_NORMAL)                \
    {                                        \
        VERT[IND++] = X;                     \
        VERT[IND++] = Y;                     \
        VERT[IND++] = Z;                     \
    }

#define SET_VERTEX(VERT, IND, X, Y, Z) \
    VERT[IND++] = X;                         \
    VERT[IND++] = Y;                         \
    VERT[IND++] = Z;

void xPrimTriangle(ScePspFVector3* v0, ScePspFVector3* v1, ScePspFVector3* v2, int attr)
{
    xGuSaveStates();
    if (attr & X_PRIM_NO_CULL) sceGuDisable(GU_CULL_FACE);
    float* vertices = (float*)sceGuGetMemory(3*attr_vert_size(attr));
    int index_f = 0;
    ScePspFVector3 normal;
    if (attr & X_PRIM_NORMAL) x_normal(&normal, v0, v1, v2);
    SET_TEXTURE(attr, vertices, index_f, 0.0f, 0.0f);
    SET_NORMAL(attr, vertices, index_f, normal.x, normal.y, normal.z);
    SET_VERTEX(vertices, index_f, v0->x, v0->y, v0->z);
    SET_TEXTURE(attr, vertices, index_f, 0.5f, 1.0f);
    SET_NORMAL(attr, vertices, index_f, normal.x, normal.y, normal.z);
    SET_VERTEX(vertices, index_f, v1->x, v1->y, v1->z);
    SET_TEXTURE(attr, vertices, index_f, 1.0f, 0.0f);
    SET_NORMAL(attr, vertices, index_f, normal.x, normal.y, normal.z);
    SET_VERTEX(vertices, index_f, v2->x, v2->y, v2->z);
    sceGumDrawArray(GU_TRIANGLES, attr_vtype(attr)|GU_TRANSFORM_3D, 3, 0, vertices);
    xGuLoadStates();
}

void xPrimPlane(float width, float height, int attr)
{
    xGuSaveStates();
    if (attr & X_PRIM_NO_CULL) sceGuDisable(GU_CULL_FACE);
    float* vertices = (float*)sceGuGetMemory(4*attr_vert_size(attr));
    int index_f = 0;
    SET_TEXTURE(attr, vertices, index_f, 0.0f, 0.0f);
    SET_NORMAL(attr, vertices, index_f, 0.0f, 0.0f, 1.0f);
    SET_VERTEX(vertices, index_f, -0.5f*width, -0.5f*height, 0.0f);
    SET_TEXTURE(attr, vertices, index_f, 0.0f, 1.0f);
    SET_NORMAL(attr, vertices, index_f, 0.0f, 0.0f, 1.0f);
    SET_VERTEX(vertices, index_f, -0.5f*width, 0.5f*height, 0.0f);
    SET_TEXTURE(attr, vertices, index_f, 1.0f, 1.0f);
    SET_NORMAL(attr, vertices, index_f, 0.0f, 0.0f, 1.0f);
    SET_VERTEX(vertices, index_f, 0.5f*width, 0.5f*height, 0.0f);
    SET_TEXTURE(attr, vertices, index_f, 1.0f, 0.0f);
    SET_NORMAL(attr, vertices, index_f, 0.0f, 0.0f, 1.0f);
    SET_VERTEX(vertices, index_f, 0.5f*width, -0.5f*height, 0.0f);
    sceGumDrawArray(GU_TRIANGLE_FAN, attr_vtype(attr)|GU_TRANSFORM_3D, 4, 0, vertices);
    xGuLoadStates();
}

void xPrimBox(float x_length, float y_length, float z_length, int attr)
{
    xGuSaveStates();
    if (attr & X_PRIM_NO_CULL) sceGuDisable(GU_CULL_FACE);
    float* vertices = (float*)sceGuGetMemory(4*6*attr_vert_size(attr));
    int index_f = 0;
    int i;
    for (i = 0; i < 2; i++)
    {
        SET_TEXTURE(attr, vertices, index_f, (i == 0 ? 0.0f : 1.0f), 0.0f);
        SET_NORMAL(attr, vertices, index_f, (i == 0 ? -1.0f : 1.0f), 0.0f, 0.0f);
        SET_VERTEX(vertices, index_f, (i == 0 ? -0.5f : 0.5f)*x_length, -0.5f*y_length, -0.5f*z_length);
        SET_TEXTURE(attr, vertices, index_f, (i == 0 ? 0.0f : 1.0f), 1.0f);
        SET_NORMAL(attr, vertices, index_f, (i == 0 ? -1.0f : 1.0f), 0.0f, 0.0f);
        SET_VERTEX(vertices, index_f, (i == 0 ? -0.5f : 0.5f)*x_length, 0.5f*y_length, -0.5f*z_length);
        SET_TEXTURE(attr, vertices, index_f, (i == 0 ? 1.0f : 0.0f), 1.0f);
        SET_NORMAL(attr, vertices, index_f, (i == 0 ? -1.0f : 1.0f), 0.0f, 0.0f);
        SET_VERTEX(vertices, index_f, (i == 0 ? -0.5f : 0.5f)*x_length, 0.5f*y_length, 0.5f*z_length);
        SET_TEXTURE(attr, vertices, index_f, (i == 0 ? 1.0f : 0.0f), 0.0f);
        SET_NORMAL(attr, vertices, index_f, (i == 0 ? -1.0f : 1.0f), 0.0f, 0.0f);
        SET_VERTEX(vertices, index_f, (i == 0 ? -0.5f : 0.5f)*x_length, -0.5f*y_length, 0.5f*z_length);
        if (i == 0) sceGuFrontFace(GU_CW);
        else sceGuFrontFace(GU_CCW);
        sceGumDrawArray(GU_TRIANGLE_FAN, attr_vtype(attr)|GU_TRANSFORM_3D, 4, 0, (void*)((u32)vertices + (0 + i)*4*attr_vert_size(attr)));
    }
    for (i = 0; i < 2; i++)
    {
        SET_TEXTURE(attr, vertices, index_f, (i == 0 ? 0.0f : 1.0f), 0.0f);
        SET_NORMAL(attr, vertices, index_f, 0.0f, (i == 0 ? -1.0f : 1.0f), 0.0f);
        SET_VERTEX(vertices, index_f, 0.5f*x_length, (i == 0 ? -0.5f : 0.5f)*y_length, -0.5f*z_length);
        SET_TEXTURE(attr, vertices, index_f, (i == 0 ? 0.0f : 1.0f), 1.0f);
        SET_NORMAL(attr, vertices, index_f, 0.0f, (i == 0 ? -1.0f : 1.0f), 0.0f);
        SET_VERTEX(vertices, index_f, -0.5f*x_length, (i == 0 ? -0.5f : 0.5f)*y_length, -0.5f*z_length);
        SET_TEXTURE(attr, vertices, index_f, (i == 0 ? 1.0f : 0.0f), 1.0f);
        SET_NORMAL(attr, vertices, index_f, 0.0f, (i == 0 ? -1.0f : 1.0f), 0.0f);
        SET_VERTEX(vertices, index_f, -0.5f*x_length, (i == 0 ? -0.5f : 0.5f)*y_length, 0.5f*z_length);
        SET_TEXTURE(attr, vertices, index_f, (i == 0 ? 1.0f : 0.0f), 0.0f);
        SET_NORMAL(attr, vertices, index_f, 0.0f, (i == 0 ? -1.0f : 1.0f), 0.0f);
        SET_VERTEX(vertices, index_f, 0.5f*x_length, (i == 0 ? -0.5f : 0.5f)*y_length, 0.5f*z_length);
        if (i == 0) sceGuFrontFace(GU_CW);
        else sceGuFrontFace(GU_CCW);
        sceGumDrawArray(GU_TRIANGLE_FAN, attr_vtype(attr)|GU_TRANSFORM_3D, 4, 0, (void*)((u32)vertices + (2 + i)*4*attr_vert_size(attr)));
    }
    for (i = 0; i < 2; i++)
    {
        SET_TEXTURE(attr, vertices, index_f, (i == 0 ? 0.0f : 1.0f), 0.0f);
        SET_NORMAL(attr, vertices, index_f, 0.0f, 0.0f, (i == 0 ? -1.0f : 1.0f));
        SET_VERTEX(vertices, index_f, -0.5f*x_length, 0.5f*y_length, (i == 0 ? -0.5f : 0.5f)*z_length);
        SET_TEXTURE(attr, vertices, index_f, (i == 0 ? 0.0f : 1.0f), 1.0f);
        SET_NORMAL(attr, vertices, index_f, 0.0f, 0.0f, (i == 0 ? -1.0f : 1.0f));
        SET_VERTEX(vertices, index_f, -0.5f*x_length, -0.5f*y_length, (i == 0 ? -0.5f : 0.5f)*z_length);
        SET_TEXTURE(attr, vertices, index_f, (i == 0 ? 1.0f : 0.0f), 1.0f);
        SET_NORMAL(attr, vertices, index_f, 0.0f, 0.0f, (i == 0 ? -1.0f : 1.0f));
        SET_VERTEX(vertices, index_f, 0.5f*x_length, -0.5f*y_length, (i == 0 ? -0.5f : 0.5f)*z_length);
        SET_TEXTURE(attr, vertices, index_f, (i == 0 ? 1.0f : 0.0f), 0.0f);
        SET_NORMAL(attr, vertices, index_f, 0.0f, 0.0f, (i == 0 ? -1.0f : 1.0f));
        SET_VERTEX(vertices, index_f, 0.5f*x_length, 0.5f*y_length, (i == 0 ? -0.5f : 0.5f)*z_length);
        if (i == 0) sceGuFrontFace(GU_CW);
        else sceGuFrontFace(GU_CCW);
        sceGumDrawArray(GU_TRIANGLE_FAN, attr_vtype(attr)|GU_TRANSFORM_3D, 4, 0, (void*)((u32)vertices + (4 + i)*4*attr_vert_size(attr)));
    }
    sceGuFrontFace(GU_CW);
    xGuLoadStates();
}

inline void xPrimCube(float length, int attr)
{
    xPrimBox(length, length, length, attr);
}

static void drawUnitSphere(int slices, int rows, int attr)
{
    xGuSaveStates();
    if (attr & X_PRIM_NO_CULL) sceGuDisable(GU_CULL_FACE);
    slices += 1;
    float* vertices = (float*)sceGuGetMemory((slices*rows)*attr_vert_size(attr));
    u16* indices = (u16*)sceGuGetMemory((6*(slices-1)*(rows-1))*sizeof(u16));

    float* slice_cos = (float*)malloc(slices*sizeof(float));
    float* slice_sin = (float*)malloc(slices*sizeof(float));
    float* row_cos = (float*)malloc(rows*sizeof(float));
    float* row_sin = (float*)malloc(rows*sizeof(float));

    float slice_inv = 1.0f/(slices-1);
    float row_inv = 1.0f/(rows-1);

    int i, j, i1, j1;

    for (i = 0; i < slices; i++)
    {
        x_sincos(i*2*X_PI*slice_inv, &slice_sin[i], &slice_cos[i]);
    }
    for (i = 0; i < rows; i++)
    {
        x_sincos(i*X_PI*row_inv, &row_sin[i], &row_cos[i]);
    }

    int index = 0;

    for (i = 0; i < slices; i++)
    {
        for (j = 0; j < rows; j++)
        {
            SET_TEXTURE(attr, vertices, index, i*slice_inv, j*row_inv);
            SET_NORMAL(attr, vertices, index, slice_cos[i]*row_sin[j], slice_sin[i]*row_sin[j], row_cos[j]);
            SET_VERTEX(vertices, index, slice_cos[i]*row_sin[j], slice_sin[i]*row_sin[j], row_cos[j]);
        }
    }

    free(slice_cos);
    free(slice_sin);
    free(row_cos);
    free(row_sin);

    index = 0;

    for (i = 0; i < slices-1; i++)
    {
        for (j = 0; j < rows-1; j++)
        {
            i1 = i+1;
            j1 = j+1;

    		indices[index++] = i * rows + j;
    		indices[index++] = i1 * rows + j1;
    		indices[index++] = i * rows + j1;

    		indices[index++] = i * rows + j;
    		indices[index++] = i1 * rows + j;
    		indices[index++] = i1 * rows + j1;
        }
    }

    sceGumDrawArray(GU_TRIANGLES, attr_vtype(attr)|GU_INDEX_16BIT|GU_TRANSFORM_3D, 6*(slices-1)*(rows-1), indices, vertices);
    xGuLoadStates();
}

inline void xPrimEllipsoid(float x_radius, float y_radius, float z_radius, int slices, int rows, int attr)
{
    sceGumPushMatrix();
    xGumScale(x_radius, y_radius, z_radius);
    drawUnitSphere(slices, rows, attr);
    sceGumPopMatrix();
}

inline void xPrimSphere(float radius, int slices, int rows, int attr)
{
    xPrimEllipsoid(radius, radius, radius, slices, rows, attr);
}

void xPrimPyramid(float length, float width, float height, int attr)
{
    xGuSaveStates();
    if (attr & X_PRIM_NO_CULL) sceGuDisable(GU_CULL_FACE);
    float* vertices = (float*)sceGuGetMemory((4*3 + 2*3)*attr_vert_size(attr));
    int index_f = 0;
    ScePspFVector3 normal, p1, p2, p3;

    SET_TEXTURE(attr, vertices, index_f, 0.0f, 0.0f);
    SET_NORMAL(attr, vertices, index_f, 0.0f, 0.0f, -1.0f);
    SET_VERTEX(vertices, index_f, -0.5f*length, 0.5f*width, -0.5f*height);
    SET_TEXTURE(attr, vertices, index_f, 0.0f, 1.0f);
    SET_NORMAL(attr, vertices, index_f, 0.0f, 0.0f, -1.0f);
    SET_VERTEX(vertices, index_f, -0.5f*length, -0.5f*width, -0.5f*height);
    SET_TEXTURE(attr, vertices, index_f, 1.0f, 1.0f);
    SET_NORMAL(attr, vertices, index_f, 0.0f, 0.0f, -1.0f);
    SET_VERTEX(vertices, index_f, 0.5f*length, -0.5f*width, -0.5f*height);

    SET_TEXTURE(attr, vertices, index_f, 1.0f, 1.0f);
    SET_NORMAL(attr, vertices, index_f, 0.0f, 0.0f, -1.0f);
    SET_VERTEX(vertices, index_f, 0.5f*length, -0.5f*width, -0.5f*height);
    SET_TEXTURE(attr, vertices, index_f, 1.0f, 0.0f);
    SET_NORMAL(attr, vertices, index_f, 0.0f, 0.0f, -1.0f);
    SET_VERTEX(vertices, index_f, 0.5f*length, 0.5f*width, -0.5f*height);
    SET_TEXTURE(attr, vertices, index_f, 0.0f, 0.0f);
    SET_NORMAL(attr, vertices, index_f, 0.0f, 0.0f, -1.0f);
    SET_VERTEX(vertices, index_f, -0.5f*length, 0.5f*width, -0.5f*height);

    int i;
    for (i = 0; i < 4; i++)
    {
        if (i == 0)
        {
            SET_VECTOR(p1,  0.5f*length, -0.5f*width, -0.5f*height);
            SET_VECTOR(p2, -0.5f*length, -0.5f*width, -0.5f*height);
        }
        else if (i == 1)
        {
            SET_VECTOR(p1, -0.5f*length, -0.5f*width, -0.5f*height);
            SET_VECTOR(p2, -0.5f*length,  0.5f*width, -0.5f*height);
        }
        else if (i == 2)
        {
            SET_VECTOR(p1, -0.5f*length,  0.5f*width, -0.5f*height);
            SET_VECTOR(p2,  0.5f*length,  0.5f*width, -0.5f*height);
        }
        else //if (i == 3)
        {
            SET_VECTOR(p1,  0.5f*length,  0.5f*width, -0.5f*height);
            SET_VECTOR(p2,  0.5f*length, -0.5f*width, -0.5f*height);
        }
        SET_VECTOR(p3, 0.0f, 0.0f, 0.5f*height);
        if (attr & X_PRIM_NORMAL) x_normal(&normal, &p1, &p2, &p3);
        SET_TEXTURE(attr, vertices, index_f, 0.0f, 0.0f);
        SET_NORMAL(attr, vertices, index_f, normal.x, normal.y, normal.z);
        SET_VERTEX(vertices, index_f, p1.x, p1.y, p1.z);
        SET_TEXTURE(attr, vertices, index_f, 0.5f, 1.0f);
        SET_NORMAL(attr, vertices, index_f, normal.x, normal.y, normal.z);
        SET_VERTEX(vertices, index_f, p2.x, p2.y, p2.z);
        SET_TEXTURE(attr, vertices, index_f, 1.0f, 0.0f);
        SET_NORMAL(attr, vertices, index_f, normal.x, normal.y, normal.z);
        SET_VERTEX(vertices, index_f, p3.x, p3.y, p3.z);
    }

    sceGumDrawArray(GU_TRIANGLES, attr_vtype(attr)|GU_TRANSFORM_3D, 18, 0, vertices);
    xGuLoadStates();
}

void xPrimCone(float radius, float height, int slices, int attr)
{
    xGuSaveStates();
    if (attr & X_PRIM_NO_CULL) sceGuDisable(GU_CULL_FACE);
    slices += 1;
    float* vertices = (float*)sceGuGetMemory((2*3*(slices-1))*attr_vert_size(attr));

    float* slice_cos = (float*)malloc(slices*sizeof(float));
    float* slice_sin = (float*)malloc(slices*sizeof(float));

    float slice_inv = 1.0f/(slices-1);

    int i, i1, index;

    for (i = 0; i < slices; i++)
    {
        x_sincos(i*2*X_PI*slice_inv, &slice_sin[i], &slice_cos[i]);
    }

    index = 0;
    for (i = 0; i < slices-1; i++)
    {
        i1 = i+1;

        SET_TEXTURE(attr, vertices, index, i*slice_inv, 0.0f);
        SET_NORMAL(attr, vertices, index, 0.0f, 0.0f, -1.0f);
        SET_VERTEX(vertices, index, radius*slice_cos[i], radius*slice_sin[i], -0.5f*height);
        SET_TEXTURE(attr, vertices, index, i1*slice_inv, 0.0f);
        SET_NORMAL(attr, vertices, index, 0.0f, 0.0f, -1.0f);
        SET_VERTEX(vertices, index, radius*slice_cos[i1], radius*slice_sin[i1], -0.5f*height);
        SET_TEXTURE(attr, vertices, index, (i+0.5f)*slice_inv, 1.0f);
        SET_NORMAL(attr, vertices, index, 0.0f, 0.0f, -1.0f);
        SET_VERTEX(vertices, index, 0.0f, 0.0f, -0.5f*height);
    }

    ScePspFVector3 normal, p1, p2, p3;

    for (i = 0; i < slices-1; i++)
    {
        i1 = i+1;

        SET_VECTOR(p1, radius*slice_cos[i1], radius*slice_sin[i1], -0.5f*height);
        SET_VECTOR(p2, radius*slice_cos[i], radius*slice_sin[i], -0.5f*height);
        SET_VECTOR(p3, 0.0f, 0.0f, 0.5f*height);

        if (attr & X_PRIM_NORMAL) x_normal(&normal, &p1, &p2, &p3);

        SET_TEXTURE(attr, vertices, index, i1*slice_inv, 0.0f);
        SET_NORMAL(attr, vertices, index, normal.x, normal.y, normal.z);
        SET_VERTEX(vertices, index, p1.x, p1.y, p1.z);
        SET_TEXTURE(attr, vertices, index, i*slice_inv, 0.0f);
        SET_NORMAL(attr, vertices, index, normal.x, normal.y, normal.z);
        SET_VERTEX(vertices, index, p2.x, p2.y, p2.z);
        SET_TEXTURE(attr, vertices, index, (i+0.5f)*slice_inv, 1.0f);
        SET_NORMAL(attr, vertices, index, normal.x, normal.y, normal.z);
        SET_VERTEX(vertices, index, p3.x, p3.y, p3.z);
    }

    free(slice_cos);
    free(slice_sin);

    sceGumDrawArray(GU_TRIANGLES, attr_vtype(attr)|GU_TRANSFORM_3D, 2*3*(slices-1), 0, vertices);
    xGuLoadStates();
}

void xPrimCylinder(float radius, float height, int slices, int attr)
{
    xGuSaveStates();
    if (attr & X_PRIM_NO_CULL) sceGuDisable(GU_CULL_FACE);
    slices += 1;
    float* vertices = (float*)sceGuGetMemory(((6+2*3)*(slices-1))*attr_vert_size(attr));

    float* slice_cos = (float*)malloc(slices*sizeof(float));
    float* slice_sin = (float*)malloc(slices*sizeof(float));

    float slice_inv = 1.0f/(slices-1);

    int i, i1, index;

    for (i = 0; i < slices; i++)
    {
        x_sincos(i*2*X_PI*slice_inv, &slice_sin[i], &slice_cos[i]);
    }

    index = 0;
    for (i = 0; i < slices-1; i++)
    {
        i1 = i+1;

        SET_TEXTURE(attr, vertices, index, i*slice_inv, 0.0f);
        SET_NORMAL(attr, vertices, index, 0.0f, 0.0f, -1.0f);
        SET_VERTEX(vertices, index, radius*slice_cos[i], radius*slice_sin[i], -0.5f*height);
        SET_TEXTURE(attr, vertices, index, i1*slice_inv, 0.0f);
        SET_NORMAL(attr, vertices, index, 0.0f, 0.0f, -1.0f);
        SET_VERTEX(vertices, index, radius*slice_cos[i1], radius*slice_sin[i1], -0.5f*height);
        SET_TEXTURE(attr, vertices, index, (i+0.5f)*slice_inv, 1.0f);
        SET_NORMAL(attr, vertices, index, 0.0f, 0.0f, -1.0f);
        SET_VERTEX(vertices, index, 0.0f, 0.0f, -0.5f*height);
    }

    for (i = 0; i < slices-1; i++)
    {
        i1 = i+1;

        SET_TEXTURE(attr, vertices, index, i1*slice_inv, 0.0f);
        SET_NORMAL(attr, vertices, index, 0.0f, 0.0f, 1.0f);
        SET_VERTEX(vertices, index, radius*slice_cos[i1], radius*slice_sin[i1], 0.5f*height);
        SET_TEXTURE(attr, vertices, index, i*slice_inv, 0.0f);
        SET_NORMAL(attr, vertices, index, 0.0f, 0.0f, 1.0f);
        SET_VERTEX(vertices, index, radius*slice_cos[i], radius*slice_sin[i], 0.5f*height);
        SET_TEXTURE(attr, vertices, index, (i+0.5f)*slice_inv, 1.0f);
        SET_NORMAL(attr, vertices, index, 0.0f, 0.0f, 1.0f);
        SET_VERTEX(vertices, index, 0.0f, 0.0f, 0.5f*height);
    }

    for (i = 0; i < slices-1; i++)
    {
        i1 = i+1;

        SET_TEXTURE(attr, vertices, index, i1*slice_inv, 0.0f);
        SET_NORMAL(attr, vertices, index, slice_cos[i1], slice_sin[i1], 0.0f);
        SET_VERTEX(vertices, index, radius*slice_cos[i1], radius*slice_sin[i1], -0.5f*height);
        SET_TEXTURE(attr, vertices, index, i*slice_inv, 0.0f);
        SET_NORMAL(attr, vertices, index, slice_cos[i], slice_sin[i], 0.0f);
        SET_VERTEX(vertices, index, radius*slice_cos[i], radius*slice_sin[i], -0.5f*height);
        SET_TEXTURE(attr, vertices, index, i*slice_inv, 1.0f);
        SET_NORMAL(attr, vertices, index, slice_cos[i], slice_sin[i], 0.0f);
        SET_VERTEX(vertices, index, radius*slice_cos[i], radius*slice_sin[i], 0.5f*height);

        SET_TEXTURE(attr, vertices, index, i*slice_inv, 1.0f);
        SET_NORMAL(attr, vertices, index, slice_cos[i], slice_sin[i], 0.0f);
        SET_VERTEX(vertices, index, radius*slice_cos[i], radius*slice_sin[i], 0.5f*height);
        SET_TEXTURE(attr, vertices, index, i1*slice_inv, 1.0f);
        SET_NORMAL(attr, vertices, index, slice_cos[i1], slice_sin[i1], 0.0f);
        SET_VERTEX(vertices, index, radius*slice_cos[i1], radius*slice_sin[i1], 0.5f*height);
        SET_TEXTURE(attr, vertices, index, i1*slice_inv, 0.0f);
        SET_NORMAL(attr, vertices, index, slice_cos[i1], slice_sin[i1], 0.0f);
        SET_VERTEX(vertices, index, radius*slice_cos[i1], radius*slice_sin[i1], -0.5f*height);
    }

    free(slice_cos);
    free(slice_sin);

    sceGumDrawArray(GU_TRIANGLES, attr_vtype(attr)|GU_TRANSFORM_3D, (6+2*3)*(slices-1), 0, vertices);
    xGuLoadStates();
}

void xPrimTorus(float radius, float thickness, int slices, int rows, int attr)
{
    xGuSaveStates();
    if (attr & X_PRIM_NO_CULL) sceGuDisable(GU_CULL_FACE);
    slices += 1;
    rows += 1;
    float* vertices = (float*)sceGuGetMemory(slices*rows*attr_vert_size(attr));
    u16* indices = (u16*)sceGuGetMemory(6*(slices-1)*(rows-1)*sizeof(u16));

    float* slice_cos = (float*)malloc(slices*sizeof(float));
    float* slice_sin = (float*)malloc(slices*sizeof(float));
    float* row_cos = (float*)malloc(rows*sizeof(float));
    float* row_sin = (float*)malloc(rows*sizeof(float));

    float slice_inv = 1.0f/(slices-1);
    float row_inv = 1.0f/(rows-1);

    int i, j, i1, j1, index;

    for (i = 0; i < slices; i++)
    {
        x_sincos(i*2*X_PI*slice_inv, &slice_sin[i], &slice_cos[i]);
    }
    for (i = 0; i < rows; i++)
    {
        x_sincos(i*2*X_PI*row_inv, &row_sin[i], &row_cos[i]);
    }

    index = 0;
    for (i = 0; i < slices; i++)
    {
    	for (j = 0; j < rows; j++)
    	{
            SET_TEXTURE(attr, vertices, index, i*slice_inv, j*row_inv); //slice_cos[i]*row_cos[j], slice_cos[i]*row_sin[j]);
            SET_NORMAL(attr, vertices, index, slice_cos[i]*row_cos[j], slice_sin[i]*row_cos[j], row_sin[j]);
            SET_VERTEX(vertices, index, (radius + thickness*row_cos[j]) * slice_cos[i], (radius + thickness*row_cos[j]) * slice_sin[i], thickness*row_sin[j]);
    	}
    }

    free(slice_cos);
    free(slice_sin);
    free(row_cos);
    free(row_sin);

    index = 0;
    for (i = 0; i < slices-1; i++)
    {
        for (j = 0; j < rows-1; j++)
        {
            i1 = i+1;
            j1 = j+1;

    		indices[index++] = i * rows + j;
    		indices[index++] = i * rows + j1;
    		indices[index++] = i1 * rows + j1;

    		indices[index++] = i1 * rows + j1;
    		indices[index++] = i1 * rows + j;
    		indices[index++] = i * rows + j;
        }
    }

    sceGumDrawArray(GU_TRIANGLES, attr_vtype(attr)|GU_INDEX_16BIT|GU_TRANSFORM_3D, 6*(slices-1)*(rows-1), indices, vertices);
    xGuLoadStates();
}
