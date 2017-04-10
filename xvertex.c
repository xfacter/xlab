/**
 * This file belongs to the 'xlab' game engine.
 * Copyright 2009 xfacter
 * Copyright 2016 wickles
 * This work is licensed under the LGPLv3
 * subject to all terms as reproduced in the included LICENSE file.
 */

#include <string.h>
#include <stdlib.h>
#include <pspgu.h>
#include "xmath.h"

#include "xvertex.h"

int texture_bits(int vtype)
{
    return (vtype & GU_TEXTURE_BITS);
}

int color_bits(int vtype)
{
    return (vtype & GU_COLOR_BITS);
}

int normal_bits(int vtype)
{
    return (vtype & GU_NORMAL_BITS);
}

int vertex_bits(int vtype)
{
    return (vtype & GU_VERTEX_BITS);
}

int texture_offset(int vtype)
{
    vtype = texture_bits(vtype);
    int offset = 0;
    switch (vtype)
    {
        case GU_TEXTURE_8BIT:
            offset = 2*sizeof(u8);
            break;
        case GU_TEXTURE_16BIT:
            offset = 2*sizeof(u16);
            break;
        case GU_TEXTURE_32BITF:
            offset = 2*sizeof(float);
            break;
        default:
            break;
    }
    return offset;
}

int color_offset(int vtype)
{
    vtype = color_bits(vtype);
    int offset = 0;
    switch (vtype)
    {
        case GU_COLOR_5650:
        case GU_COLOR_5551:
        case GU_COLOR_4444:
            offset = sizeof(u16);
            break;
        case GU_COLOR_8888:
            offset = sizeof(u32);
            break;
        default:
            break;
    }
    return offset;
}

int normal_offset(int vtype)
{
    vtype = normal_bits(vtype);
    int offset = 0;
    switch (vtype)
    {
        case GU_NORMAL_8BIT:
            offset = 3*sizeof(u8);
            break;
        case GU_NORMAL_16BIT:
            offset = 3*sizeof(u16);
            break;
        case GU_NORMAL_32BITF:
            offset = 3*sizeof(float);
            break;
        default:
            break;
    }
    return offset;
}

int vertex_offset(int vtype)
{
    vtype = vertex_bits(vtype);
    int offset = 0;
    switch (vtype)
    {
        case GU_VERTEX_8BIT:
            offset = 3*sizeof(s8);
            break;
        case GU_VERTEX_16BIT:
            offset = 3*sizeof(u16);
            break;
        case GU_VERTEX_32BITF:
            offset = 3*sizeof(float);
            break;
        default:
            break;
    }
    return offset;
}

int total_offset(int vtype)
{
    return (texture_offset(vtype) + color_offset(vtype) + normal_offset(vtype) + vertex_offset(vtype));
}

void xVertSoftNormals(int vtype, int count, void* vertices)
{
    if ((vtype & GU_NORMAL_BITS) != GU_NORMAL_32BITF || (vtype & GU_VERTEX_BITS) != GU_VERTEX_32BITF) return;
    int n_offset = texture_offset(vtype) + color_offset(vtype);
    int v_offset = n_offset + normal_offset(vtype);
    float x, y, z;
    float *nx, *ny, *nz;
    int i;
    for (i = 0; i < count; i++)
    {
        x = *(float*)((u32)vertices + v_offset + 0*sizeof(float));
        y = *(float*)((u32)vertices + v_offset + 1*sizeof(float));
        z = *(float*)((u32)vertices + v_offset + 2*sizeof(float));
        nx = (float*)((u32)vertices + n_offset + 0*sizeof(float));
        ny = (float*)((u32)vertices + n_offset + 1*sizeof(float));
        nz = (float*)((u32)vertices + n_offset + 2*sizeof(float));

        float mag = x_sqrtf(SQR(x) + SQR(y) + SQR(z));
        if (mag > 0)
        {
            float inv_mag = 1.0f/mag;
            *nx = x * inv_mag;
            *ny = y * inv_mag;
            *nz = z * inv_mag;
        }
        vertices = (void*)((u32)vertices + total_offset(vtype));
    }
}

void xVertHardNormals(int prim, int vtype, int count, void* vertices)
{
    if ((vtype & GU_NORMAL_BITS) != GU_NORMAL_32BITF || (vtype & GU_VERTEX_BITS) != GU_VERTEX_32BITF) return;
    //temporary? blocking other primitive types
    if (prim != GU_TRIANGLES) return;
    int n_offset = texture_offset(vtype) + color_offset(vtype);
    int v_offset = n_offset + normal_offset(vtype);
    ScePspFVector3 n, p1, p2, p3;
    float *nx, *ny, *nz;
    int i;
    for (i = 0; i < count/3; i++)
    {
        p1.x = *(float*)((u32)vertices + 0*total_offset(vtype) + v_offset + 0*sizeof(float));
        p1.y = *(float*)((u32)vertices + 0*total_offset(vtype) + v_offset + 1*sizeof(float));
        p1.z = *(float*)((u32)vertices + 0*total_offset(vtype) + v_offset + 2*sizeof(float));
        p2.x = *(float*)((u32)vertices + 1*total_offset(vtype) + v_offset + 0*sizeof(float));
        p2.y = *(float*)((u32)vertices + 1*total_offset(vtype) + v_offset + 1*sizeof(float));
        p2.z = *(float*)((u32)vertices + 1*total_offset(vtype) + v_offset + 2*sizeof(float));
        p3.x = *(float*)((u32)vertices + 2*total_offset(vtype) + v_offset + 0*sizeof(float));
        p3.y = *(float*)((u32)vertices + 2*total_offset(vtype) + v_offset + 1*sizeof(float));
        p3.z = *(float*)((u32)vertices + 2*total_offset(vtype) + v_offset + 2*sizeof(float));

        x_normal(&n, &p1, &p2, &p3);

        nx = (float*)((u32)vertices + 0*total_offset(vtype) + n_offset + 0*sizeof(float));
        ny = (float*)((u32)vertices + 0*total_offset(vtype) + n_offset + 1*sizeof(float));
        nz = (float*)((u32)vertices + 0*total_offset(vtype) + n_offset + 2*sizeof(float));

        *nx = n.x;
        *ny = n.y;
        *nz = n.z;

        nx = (float*)((u32)vertices + 1*total_offset(vtype) + n_offset + 0*sizeof(float));
        ny = (float*)((u32)vertices + 1*total_offset(vtype) + n_offset + 1*sizeof(float));
        nz = (float*)((u32)vertices + 1*total_offset(vtype) + n_offset + 2*sizeof(float));

        *nx = n.x;
        *ny = n.y;
        *nz = n.z;

        nx = (float*)((u32)vertices + 2*total_offset(vtype) + n_offset + 0*sizeof(float));
        ny = (float*)((u32)vertices + 2*total_offset(vtype) + n_offset + 1*sizeof(float));
        nz = (float*)((u32)vertices + 2*total_offset(vtype) + n_offset + 2*sizeof(float));

        *nx = n.x;
        *ny = n.y;
        *nz = n.z;

        vertices = (void*)((u32)vertices + 3*total_offset(vtype));
    }
}

void xVertTranslate(int vtype, int count, void* vertices, ScePspFVector3* translate)
{
    int v_offset = texture_offset(vtype) + color_offset(vtype) + normal_offset(vtype);
    int i;
    for (i = 0; i < count; i++)
    {
        if (vertex_bits(vtype) == GU_VERTEX_32BITF)
        {
            float *x, *y, *z;
            x = (float*)((u32)vertices + v_offset + 0*sizeof(float));
            y = (float*)((u32)vertices + v_offset + 1*sizeof(float));
            z = (float*)((u32)vertices + v_offset + 2*sizeof(float));
            *x += translate->x;
            *y += translate->y;
            *z += translate->z;
        }
        else if (vertex_bits(vtype) == GU_VERTEX_16BIT)
        {
            s16 *x, *y, *z;
            x = (s16*)((u32)vertices + v_offset + 0*sizeof(s16));
            y = (s16*)((u32)vertices + v_offset + 1*sizeof(s16));
            z = (s16*)((u32)vertices + v_offset + 2*sizeof(s16));
            *x += (s16)translate->x;
            *y += (s16)translate->y;
            *z += (s16)translate->z;
        }
        else if (vertex_bits(vtype) == GU_VERTEX_8BIT)
        {
            s8 *x, *y, *z;
            x = (s8*)((u32)vertices + v_offset + 0*sizeof(s8));
            y = (s8*)((u32)vertices + v_offset + 1*sizeof(s8));
            z = (s8*)((u32)vertices + v_offset + 2*sizeof(s8));
            *x += (s8)translate->x;
            *y += (s8)translate->y;
            *z += (s8)translate->z;
        }

        vertices = (void*)((u32)vertices + total_offset(vtype));
    }
}

void xVertRotateX(int vtype, int count, void* vertices, float radians)
{
    float sine, cosine;
    x_sincos(radians, &sine, &cosine);

    int v_offset = texture_offset(vtype) + color_offset(vtype) + normal_offset(vtype);
    int i;
    for (i = 0; i < count; i++)
    {
        if (vertex_bits(vtype) == GU_VERTEX_32BITF)
        {
            float *y, *z;
            y = (float*)((u32)vertices + v_offset + 1*sizeof(float));
            z = (float*)((u32)vertices + v_offset + 2*sizeof(float));
            float temp_y, temp_z;
            temp_y = *y;
            temp_z = *z;
            *y = (cosine * temp_y) - (sine * temp_z);
            *z = (sine * temp_y) + (cosine * temp_z);
        }
        else if (vertex_bits(vtype) == GU_VERTEX_16BIT)
        {
            s16 *y, *z;
            y = (s16*)((u32)vertices + v_offset + 1*sizeof(s16));
            z = (s16*)((u32)vertices + v_offset + 2*sizeof(s16));
            s16 temp_y, temp_z;
            temp_y = *y;
            temp_z = *z;
            *y = (s16)((cosine * temp_y) - (sine * temp_z));
            *z = (s16)((sine * temp_y) + (cosine * temp_z));
        }
        else if (vertex_bits(vtype) == GU_VERTEX_8BIT)
        {
            s8 *y, *z;
            y = (s8*)((u32)vertices + v_offset + 1*sizeof(s8));
            z = (s8*)((u32)vertices + v_offset + 2*sizeof(s8));
            s8 temp_y, temp_z;
            temp_y = *y;
            temp_z = *z;
            *y = (s8)((cosine * temp_y) - (sine * temp_z));
            *z = (s8)((sine * temp_y) + (cosine * temp_z));
        }

        vertices = (void*)((u32)vertices + total_offset(vtype));
    }
}

void xVertRotateY(int vtype, int count, void* vertices, float radians)
{
    float sine, cosine;
    x_sincos(radians, &sine, &cosine);

    int v_offset = texture_offset(vtype) + color_offset(vtype) + normal_offset(vtype);
    int i;
    for (i = 0; i < count; i++)
    {
        if (vertex_bits(vtype) == GU_VERTEX_32BITF)
        {
            float *x, *z;
            x = (float*)((u32)vertices + v_offset + 0*sizeof(float));
            z = (float*)((u32)vertices + v_offset + 2*sizeof(float));
            float temp_x, temp_z;
            temp_x = *x;
            temp_z = *z;
            *x = (cosine * temp_x) + (sine * temp_z);
            *z = -(sine * temp_x) + (cosine * temp_z);
        }
        else if (vertex_bits(vtype) == GU_VERTEX_16BIT)
        {
            s16 *x, *z;
            x = (s16*)((u32)vertices + v_offset + 0*sizeof(s16));
            z = (s16*)((u32)vertices + v_offset + 2*sizeof(s16));
            s16 temp_x, temp_z;
            temp_x = *x;
            temp_z = *z;
            *x = (s16)((cosine * temp_x) + (sine * temp_z));
            *z = (s16)(-(sine * temp_x) + (cosine * temp_z));
        }
        else if (vertex_bits(vtype) == GU_VERTEX_8BIT)
        {
            s8 *x, *z;
            x = (s8*)((u32)vertices + v_offset + 0*sizeof(s8));
            z = (s8*)((u32)vertices + v_offset + 2*sizeof(s8));
            s8 temp_x, temp_z;
            temp_x = *x;
            temp_z = *z;
            *x = (s8)((cosine * temp_x) + (sine * temp_z));
            *z = (s8)(-(sine * temp_x) + (cosine * temp_z));
        }

        vertices = (void*)((u32)vertices + total_offset(vtype));
    }
}

void xVertRotateZ(int vtype, int count, void* vertices, float radians)
{
    float sine, cosine;
    x_sincos(radians, &sine, &cosine);

    int v_offset = texture_offset(vtype) + color_offset(vtype) + normal_offset(vtype);
    int i;
    for (i = 0; i < count; i++)
    {
        if (vertex_bits(vtype) == GU_VERTEX_32BITF)
        {
            float *x, *y;
            x = (float*)((u32)vertices + v_offset + 0*sizeof(float));
            y = (float*)((u32)vertices + v_offset + 1*sizeof(float));
            float temp_x, temp_y;
            temp_x = *x;
            temp_y = *y;
            *x = (cosine * temp_x) - (sine * temp_y);
            *y = (sine * temp_x) + (cosine * temp_y);
        }
        else if (vertex_bits(vtype) == GU_VERTEX_16BIT)
        {
            s16 *x, *y;
            x = (s16*)((u32)vertices + v_offset + 0*sizeof(s16));
            y = (s16*)((u32)vertices + v_offset + 1*sizeof(s16));
            s16 temp_x, temp_y;
            temp_x = *x;
            temp_y = *y;
            *x = (s16)((cosine * temp_x) - (sine * temp_y));
            *y = (s16)((sine * temp_x) + (cosine * temp_y));
        }
        else if (vertex_bits(vtype) == GU_VERTEX_8BIT)
        {
            s8 *x, *y;
            x = (s8*)((u32)vertices + v_offset + 0*sizeof(s8));
            y = (s8*)((u32)vertices + v_offset + 1*sizeof(s8));
            s8 temp_x, temp_y;
            temp_x = *x;
            temp_y = *y;
            *x = (s8)((cosine * temp_x) - (sine * temp_y));
            *y = (s8)((sine * temp_x) + (cosine * temp_y));
        }

        vertices = (void*)((u32)vertices + total_offset(vtype));
    }
}

void* xVertNewTranslated(int vtype, int count, void* vertices, ScePspFVector3* translate)
{
    void* new_verts = malloc(count*total_offset(vtype));
    memcpy(new_verts, vertices, count*total_offset(vtype));
    xVertTranslate(vtype, count, new_verts, translate);
    return new_verts;
}

void* xVertNewRotatedX(int vtype, int count, void* vertices, float radians)
{
    void* new_verts = malloc(count*total_offset(vtype));
    memcpy(new_verts, vertices, count*total_offset(vtype));
    xVertRotateX(vtype, count, new_verts, radians);
    return new_verts;
}

void* xVertNewRotatedY(int vtype, int count, void* vertices, float radians)
{
    void* new_verts = malloc(count*total_offset(vtype));
    memcpy(new_verts, vertices, count*total_offset(vtype));
    xVertRotateY(vtype, count, new_verts, radians);
    return new_verts;
}

void* xVertNewRotatedZ(int vtype, int count, void* vertices, float radians)
{
    void* new_verts = malloc(count*total_offset(vtype));
    memcpy(new_verts, vertices, count*total_offset(vtype));
    xVertRotateZ(vtype, count, new_verts, radians);
    return new_verts;
}
