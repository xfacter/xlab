#include <string.h>
#include <pspgu.h>
#include <pspgum.h>
#include "xmem.h"
#include "xmath.h"
#include "xgraphics.h"

#include "xobj.h"

#ifdef X_DEBUG
#include "xlog.h"
#define X_LOG(format, ... ) xLogPrintf("xObj: " format, __VA_ARGS__)
#else
#define X_LOG(format, ... ) do{}while(0)
#endif

int xObjLoad(xObj* object, char* filename)
{
    if (!object) return 0;
    FILE* file = fopen(filename, "r");
    if (!file) return 0;
    
    int i;
    char buffer[256];
    ScePspFVector3* vertices;
    ScePspFVector3* normals;
    ScePspFVector3* texcoords;
    int num_verts = 0;
    int num_normals = 0;
    int num_texcoords = 0;
    int num_tris = 0;

    while(!feof(file))
    {
        fgets(buffer, sizeof(buffer), file);
        if (memcmp(buffer, "vn", 2) == 0)
        {
            num_normals += 1;
        }
        else if (memcmp(buffer, "vt", 2) == 0)
        {
            num_texcoords += 1;
        }
        else if (memcmp(buffer, "vp", 2) == 0)
        {
            //unsupported
        }
        else if (memcmp(buffer, "v", 1) == 0)
        {
            num_verts += 1;
        }
        else if (memcmp(buffer, "f", 1) == 0)
        {
            num_tris += 1;
        }
    }

    vertices = x_malloc(num_verts*sizeof(ScePspFVector3));
    normals = x_malloc(num_normals*sizeof(ScePspFVector3));
    texcoords = x_malloc(num_texcoords*sizeof(ScePspFVector3));
    
    object->vtype = 0;
    int objargs = 0;
    int size = 0;
    if (num_normals > 0)
    {
        object->vtype |= GU_NORMAL_32BITF;
        objargs += 1;
        size += 3;
    }
    if (num_texcoords > 0)
    {
        object->vtype |= GU_TEXTURE_32BITF;
        objargs += 1;
        size += 2;
    }
    if (num_verts > 0)
    {
        object->vtype |= GU_VERTEX_32BITF;
        objargs += 1;
        size += 3;
    }
    int pad = x_num_align(size, 4) - size;
    X_LOG("size: %i", size);
    X_LOG("pad: %i", pad);
    object->num_verts = num_tris*3;
    object->vertices = x_malloc(object->num_verts*(size+pad)*sizeof(float));

    int index_verts = 0;
    int index_normals = 0;
    int index_texcoords = 0;
    int indices[9];
    float* ptr = (float*)object->vertices;

    rewind(file);

    while(!feof(file))
    {
        fgets(buffer, sizeof(buffer), file);
        if (memcmp(buffer, "vn", 2) == 0)
        {
            sscanf(buffer, "vn %f %f %f", &normals[index_normals].x, &normals[index_normals].y, &normals[index_normals].z);
            index_normals += 1;
        }
        else if (memcmp(buffer, "vt", 2) == 0)
        {
            sscanf(buffer, "vt %f %f %f", &texcoords[index_texcoords].x, &texcoords[index_texcoords].y, &texcoords[index_texcoords].z);
            index_texcoords += 1;
        }
        else if (memcmp(buffer, "vp", 2) == 0)
        {
            //unsupported
        }
        else if (memcmp(buffer, "v", 1) == 0)
        {
            sscanf(buffer, "v %f %f %f", &vertices[index_verts].x, &vertices[index_verts].y, &vertices[index_verts].z);
            index_verts += 1;
        }
        else if (memcmp(buffer, "f", 1) == 0)
        {
            if (objargs == 1)
            {
                sscanf(buffer, "f %i %i %i", &indices[0], &indices[1], &indices[2]);
                if (num_texcoords > 0)
                {
                    for (i = 0; i < 3; i++)
                    {
                        *(ptr++) = texcoords[indices[i]-1].x; //vert.u
                        *(ptr++) = texcoords[indices[i]-1].y; //vert.v
                        ptr += pad;
                    }
                }
                else if (num_normals > 0)
                {
                    for (i = 0; i < 3; i++)
                    {
                        *(ptr++) = normals[indices[i]-1].x; //vert.nx
                        *(ptr++) = normals[indices[i]-1].y; //vert.ny
                        *(ptr++) = normals[indices[i]-1].z; //vert.nz
                        ptr += pad;
                    }
                }
                else if (num_verts > 0)
                {
                    for (i = 0; i < 3; i++)
                    {
                        *(ptr++) = vertices[indices[i]-1].x; //vert.x
                        *(ptr++) = vertices[indices[i]-1].y; //vert.y
                        *(ptr++) = vertices[indices[i]-1].z; //vert.z
                        ptr += pad;
                    }
                }
            }
            else if (objargs == 2)
            {
                sscanf(buffer, "f %i/%i %i/%i %i/%i", &indices[0], &indices[1], &indices[2], &indices[3], &indices[4], &indices[5]);
                for (i = 0; i < 3; i++)
                {
                    int index = 0;
                    if (num_texcoords > 0)
                    {
                        *(ptr++) = texcoords[indices[i*2+index]-1].x; //vert.u
                        *(ptr++) = texcoords[indices[i*2+index]-1].y; //vert.v
                        index += 1;
                    }
                    if (num_normals > 0)
                    {
                        *(ptr++) = normals[indices[i*2+index]-1].x; //vert.nx
                        *(ptr++) = normals[indices[i*2+index]-1].y; //vert.ny
                        *(ptr++) = normals[indices[i*2+index]-1].z; //vert.nz
                        index += 1;
                    }
                    if (num_verts > 0)
                    {
                        *(ptr++) = vertices[indices[i*2+index]-1].x; //vert.x
                        *(ptr++) = vertices[indices[i*2+index]-1].y; //vert.y
                        *(ptr++) = vertices[indices[i*2+index]-1].z; //vert.z
                        index += 1;
                    }
                    ptr += pad;
                }
            }
            else if (objargs == 3)
            {
                sscanf(buffer, "f %i/%i/%i %i/%i/%i %i/%i/%i",
                       &indices[0], &indices[1], &indices[2],
                       &indices[3], &indices[4], &indices[5],
                       &indices[6], &indices[7], &indices[8]);
                for (i = 0; i < 3; i++)
                {
                    *(ptr++) = texcoords[indices[i*3+1]-1].x; //vert.u
                    *(ptr++) = texcoords[indices[i*3+1]-1].y; //vert.v
                    *(ptr++) = normals[indices[i*3+2]-1].x; //vert.nx
                    *(ptr++) = normals[indices[i*3+2]-1].y; //vert.ny
                    *(ptr++) = normals[indices[i*3+2]-1].z; //vert.nz
                    *(ptr++) = vertices[indices[i*3+0]-1].x; //vert.x
                    *(ptr++) = vertices[indices[i*3+0]-1].y; //vert.y
                    *(ptr++) = vertices[indices[i*3+0]-1].z; //vert.z
                    ptr += pad;
                }
            }
        }
    }

    x_free(vertices);
    x_free(normals);
    x_free(texcoords);
    
    fclose(file);

    return 1;
}

void xObjFree(xObj* object)
{
    if (!object) return;
    x_free(object->vertices);
}

#define FLOAT_TO_S16(x) ((s16)(32767.f*(x)))
#define FLOAT_TO_S8(x) ((s8)(127.f*(x)))

/* doesnt seem to work */
/*
void xObjOptimize(xObj* object)
{
    if (!object) return;
    int new_vtype = 0;
    if (vertex_bits(object->vtype)) new_vtype |= GU_VERTEX_32BITF;
    if (texture_bits(object->vtype)) new_vtype |= GU_TEXTURE_8BIT;
    if (normal_bits(object->vtype)) new_vtype |= GU_NORMAL_8BIT;
    if (object->vtype == new_vtype) return;
    void* new_vertices = x_malloc(object->num_verts*total_size_pad(new_vtype));
    if (!new_vertices) return;
    void* ptr_old = object->vertices;
    void* ptr_new = new_vertices;
    float* data_old;
    s8* data_new;
    int i, j;
    for (i = 0; i < object->num_verts; i++)
    {
        if (texture_bits(object->vtype))
        {
            data_old = (float*)ptr_old;
            data_new = (s8*)ptr_new;
            for (j = 0; j < 2; j++) data_new[j] = (s8)(data_old[j]*127);
            ptr_old += 2*sizeof(float);
            ptr_new += 2*sizeof(s8);
        }
        if (normal_bits(object->vtype))
        {
            data_old = (float*)ptr_old;
            data_new = (s8*)ptr_new;
            for (j = 0; j < 3; j++) data_new[j] = (s8)(data_old[j]*127);
            ptr_old += 3*sizeof(float);
            ptr_new += 3*sizeof(s8);
        }
        if (vertex_bits(object->vtype))
        {
            //no conversion needed, possibly change later to convert, then scale and transform with saved vectors
            memcpy(ptr_new, ptr_old, 3*sizeof(float));
            ptr_old += 3*sizeof(float);
            ptr_new += 3*sizeof(float);
        }
        ptr_old += pad_offset(object->vtype);
        ptr_new += pad_offset(new_vtype);
    }
    x_free(object->vertices);
    object->vertices = new_vertices;
    object->vtype = new_vtype;
}
*/

void xObjDraw(xObj* object)
{
    if (!object) return;
    sceGumDrawArray(GU_TRIANGLES, object->vtype|GU_TRANSFORM_3D, object->num_verts, 0, object->vertices);
}
