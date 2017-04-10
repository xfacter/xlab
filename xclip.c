#include <string.h>
#include <pspgu.h>
#include <pspgum.h>
#include "xconfig.h"
#include "xmath.h"

#include "xclip.h"

static void normalize_plane(ScePspFVector4* plane)
{
    float inv_mag = x_sqrtf(SQR(plane->x) + SQR(plane->y) + SQR(plane->z));
    if (inv_mag == 0.0f)
    {
        plane->w = 0.0f;
        return;
    }
    inv_mag = 1.0f/inv_mag;
    plane->x *= inv_mag;
    plane->y *= inv_mag;
    plane->z *= inv_mag;
    plane->w *= inv_mag;
}

void xClipGetFrustum(xFrustum* out)
{
    ScePspFMatrix4 projection, view, model, clip;
    sceGumMatrixMode(GU_PROJECTION);
    sceGumStoreMatrix(&projection);
    sceGumMatrixMode(GU_VIEW);
    sceGumStoreMatrix(&view);
    sceGumMatrixMode(GU_MODEL);
    sceGumStoreMatrix(&model);
    gumMultMatrix(&clip, &projection, &view);
    gumMultMatrix(&clip, &clip, &model);

    float* t = (float*)&clip;
    int i;
    for (i = 0; i < 3; i++)
    {
        out->planes[i*2+0].x = t[0+3] + t[0+i];
        out->planes[i*2+0].y = t[4+3] + t[4+i];
        out->planes[i*2+0].z = t[8+3] + t[8+i];
        out->planes[i*2+0].w = t[12+3] + t[12+i];
        normalize_plane(&out->planes[i*2+0]);
        out->planes[i*2+1].x = t[0+3] - t[0+i];
        out->planes[i*2+1].y = t[4+3] - t[4+i];
        out->planes[i*2+1].z = t[8+3] - t[8+i];
        out->planes[i*2+1].w = t[12+3] - t[12+i];
        normalize_plane(&out->planes[i*2+1]);
    }
}

int xClipPointInFrustum(xFrustum* f, ScePspFVector3* p)
{
    int i;
    for (i = 0; i < 6; i++)
    {
        if (p->x*f->planes[i].x + p->y*f->planes[i].y + p->z*f->planes[i].z + f->planes[i].w < 0.0f)
        {
            return 0;
        }
    }
    return 1;
}

/*
int xClipDrawArrayFrustum(ScePspFVector4* planes, int prim, int vtype, int count, void* indices, void* vertices)
{
    if ((texture_bits(vtype) && texture_bits(vtype) != GU_TEXTURE_32BITF) || (vertex_bits(vtype) && vertex_bits(vtype) != GU_VERTEX_32BITF) ||
        (prim != GU_TRIANGLES && prim != GU_TRIANGLE_STRIP && prim != GU_TRIANGLE_FAN))
    {
        X_LOG("Could not clip vertices. Unsupported texture or vertex type.", 0);
        sceGumDrawArray(prim, vtype, count, indices, vertices);
        return 0;
    }
    int vert_size = total_size(vtype);
    void* clipped_verts = sceGuGetMemory(count*vert_size);
    int clipped = 0;
    int prev;
    float t0, t1, tt;
    float* ptr_clip;
    float* ptr_this;
    float* ptr_prev;
    int i, j;
    for (j = 0; j < 6; j++)
    {
        for (i = 0; i < count; i++)
        {
            if (prim == GU_TRIANGLES)
            {
                if (i % 3 == 0) prev = i + 2;
                else prev = i - 1;
            }
            else if (prim == GU_TRIANGLE_STRIP)
            {
                if (i == 0) prev = i + 1;
                else prev = i - 1;
            }
            else if (prim == GU_TRIANGLE_FAN)
            {
                if (i == 0) prev = count - 1;
                else prev = i - 1;
            }

            ptr_this = (float*)((u32)vertices + vertex_index_offset(vtype, i, indices) + vertex_offset(vtype));
            ptr_prev = (float*)((u32)vertices + vertex_index_offset(vtype, prev, indices) + vertex_offset(vtype));
            t0 = planes[j].x*ptr_prev[0] + planes[j].y*ptr_prev[1] + planes[j].z*ptr_prev[2] + planes[j].w;
            t1 = planes[j].x*ptr_this[0] + planes[j].y*ptr_this[1] + planes[j].z*ptr_this[2] + planes[j].w;

            if (((t0 >= 0) ^ (t1 >= 0)) && t0 != t1)
            {
                tt = t0/(t0-t1);
                ptr_this = (float*)((u32)clipped_verts + i*vert_size + texture_offset(vtype));
                ptr_this = (float*)((u32)vertices + vertex_index_offset(vtype, i, indices) + texture_offset(vtype));
                ptr_prev = (float*)((u32)vertices + vertex_index_offset(vtype, prev, indices) + texture_offset(vtype));
                ptr_clip[0] = ptr_prev[0] + tt*(ptr_this[0] - ptr_prev[0]);
                ptr_clip[1] = ptr_prev[1] + tt*(ptr_this[1] - ptr_prev[1]);
                ptr_this = (float*)((u32)clipped_verts + i*vert_size + vertex_offset(vtype));
                ptr_this = (float*)((u32)vertices + vertex_index_offset(vtype, i, indices) + vertex_offset(vtype));
                ptr_prev = (float*)((u32)vertices + vertex_index_offset(vtype, prev, indices) + vertex_offset(vtype));
                ptr_clip[0] = ptr_prev[0] + tt*(ptr_this[0] - ptr_prev[0]);
                ptr_clip[1] = ptr_prev[1] + tt*(ptr_this[1] - ptr_prev[1]);
                ptr_clip[2] = ptr_prev[2] + tt*(ptr_this[2] - ptr_prev[2]);
                clipped += 1;
            }
        }
    }
    sceGumDrawArray(prim, vtype & ~GU_INDEX_BITS, count, 0, clipped_verts);
    return clipped;
}
*/
