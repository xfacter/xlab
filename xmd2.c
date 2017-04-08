#include <pspgu.h>
#include <pspgum.h>
#include "xgraphics.h"
#include "xmath.h"
#include "xmem.h"

#include "xmd2.h"

#ifdef X_DEBUG
#include "xlog.h"
#define X_LOG(format, ... ) xLogPrintf("xMd2: " format, __VA_ARGS__)
#else
#define X_LOG(format, ... ) do{}while(0)
#endif

#define MD2_MAX_TRIS       (4096)
#define MD2_MAX_VERTS      (2048)
#define MD2_MAX_TEX_COORDS (2048)
#define MD2_MAX_FRAMES     (512)
#define MD2_MAX_SKINS      (32)
#define MD2_MAX_NORMALS    (162)

#define MD2_IDENT ('I'<<0|'D'<<8|'P'<<16|'2'<<24) /* "IDP2" (0x32504449) */
#define MD2_VERSION (8)

#define MD2_NORMAL_TABLE_SIZE (162)

static ScePspFVector3 md2_normal_lookup[MD2_NORMAL_TABLE_SIZE] =
{
    { -0.525731f,  0.000000f,  0.850651f },
    { -0.442863f,  0.238856f,  0.864188f },
    { -0.295242f,  0.000000f,  0.955423f },
    { -0.309017f,  0.500000f,  0.809017f },
    { -0.162460f,  0.262866f,  0.951056f },
    {  0.000000f,  0.000000f,  1.000000f },
    {  0.000000f,  0.850651f,  0.525731f },
    { -0.147621f,  0.716567f,  0.681718f },
    {  0.147621f,  0.716567f,  0.681718f },
    {  0.000000f,  0.525731f,  0.850651f },
    {  0.309017f,  0.500000f,  0.809017f },
    {  0.525731f,  0.000000f,  0.850651f },
    {  0.295242f,  0.000000f,  0.955423f },
    {  0.442863f,  0.238856f,  0.864188f },
    {  0.162460f,  0.262866f,  0.951056f },
    { -0.681718f,  0.147621f,  0.716567f },
    { -0.809017f,  0.309017f,  0.500000f },
    { -0.587785f,  0.425325f,  0.688191f },
    { -0.850651f,  0.525731f,  0.000000f },
    { -0.864188f,  0.442863f,  0.238856f },
    { -0.716567f,  0.681718f,  0.147621f },
    { -0.688191f,  0.587785f,  0.425325f },
    { -0.500000f,  0.809017f,  0.309017f },
    { -0.238856f,  0.864188f,  0.442863f },
    { -0.425325f,  0.688191f,  0.587785f },
    { -0.716567f,  0.681718f, -0.147621f },
    { -0.500000f,  0.809017f, -0.309017f },
    { -0.525731f,  0.850651f,  0.000000f },
    {  0.000000f,  0.850651f, -0.525731f },
    { -0.238856f,  0.864188f, -0.442863f },
    {  0.000000f,  0.955423f, -0.295242f },
    { -0.262866f,  0.951056f, -0.162460f },
    {  0.000000f,  1.000000f,  0.000000f },
    {  0.000000f,  0.955423f,  0.295242f },
    { -0.262866f,  0.951056f,  0.162460f },
    {  0.238856f,  0.864188f,  0.442863f },
    {  0.262866f,  0.951056f,  0.162460f },
    {  0.500000f,  0.809017f,  0.309017f },
    {  0.238856f,  0.864188f, -0.442863f },
    {  0.262866f,  0.951056f, -0.162460f },
    {  0.500000f,  0.809017f, -0.309017f },
    {  0.850651f,  0.525731f,  0.000000f },
    {  0.716567f,  0.681718f,  0.147621f },
    {  0.716567f,  0.681718f, -0.147621f },
    {  0.525731f,  0.850651f,  0.000000f },
    {  0.425325f,  0.688191f,  0.587785f },
    {  0.864188f,  0.442863f,  0.238856f },
    {  0.688191f,  0.587785f,  0.425325f },
    {  0.809017f,  0.309017f,  0.500000f },
    {  0.681718f,  0.147621f,  0.716567f },
    {  0.587785f,  0.425325f,  0.688191f },
    {  0.955423f,  0.295242f,  0.000000f },
    {  1.000000f,  0.000000f,  0.000000f },
    {  0.951056f,  0.162460f,  0.262866f },
    {  0.850651f, -0.525731f,  0.000000f },
    {  0.955423f, -0.295242f,  0.000000f },
    {  0.864188f, -0.442863f,  0.238856f },
    {  0.951056f, -0.162460f,  0.262866f },
    {  0.809017f, -0.309017f,  0.500000f },
    {  0.681718f, -0.147621f,  0.716567f },
    {  0.850651f,  0.000000f,  0.525731f },
    {  0.864188f,  0.442863f, -0.238856f },
    {  0.809017f,  0.309017f, -0.500000f },
    {  0.951056f,  0.162460f, -0.262866f },
    {  0.525731f,  0.000000f, -0.850651f },
    {  0.681718f,  0.147621f, -0.716567f },
    {  0.681718f, -0.147621f, -0.716567f },
    {  0.850651f,  0.000000f, -0.525731f },
    {  0.809017f, -0.309017f, -0.500000f },
    {  0.864188f, -0.442863f, -0.238856f },
    {  0.951056f, -0.162460f, -0.262866f },
    {  0.147621f,  0.716567f, -0.681718f },
    {  0.309017f,  0.500000f, -0.809017f },
    {  0.425325f,  0.688191f, -0.587785f },
    {  0.442863f,  0.238856f, -0.864188f },
    {  0.587785f,  0.425325f, -0.688191f },
    {  0.688191f,  0.587785f, -0.425325f },
    { -0.147621f,  0.716567f, -0.681718f },
    { -0.309017f,  0.500000f, -0.809017f },
    {  0.000000f,  0.525731f, -0.850651f },
    { -0.525731f,  0.000000f, -0.850651f },
    { -0.442863f,  0.238856f, -0.864188f },
    { -0.295242f,  0.000000f, -0.955423f },
    { -0.162460f,  0.262866f, -0.951056f },
    {  0.000000f,  0.000000f, -1.000000f },
    {  0.295242f,  0.000000f, -0.955423f },
    {  0.162460f,  0.262866f, -0.951056f },
    { -0.442863f, -0.238856f, -0.864188f },
    { -0.309017f, -0.500000f, -0.809017f },
    { -0.162460f, -0.262866f, -0.951056f },
    {  0.000000f, -0.850651f, -0.525731f },
    { -0.147621f, -0.716567f, -0.681718f },
    {  0.147621f, -0.716567f, -0.681718f },
    {  0.000000f, -0.525731f, -0.850651f },
    {  0.309017f, -0.500000f, -0.809017f },
    {  0.442863f, -0.238856f, -0.864188f },
    {  0.162460f, -0.262866f, -0.951056f },
    {  0.238856f, -0.864188f, -0.442863f },
    {  0.500000f, -0.809017f, -0.309017f },
    {  0.425325f, -0.688191f, -0.587785f },
    {  0.716567f, -0.681718f, -0.147621f },
    {  0.688191f, -0.587785f, -0.425325f },
    {  0.587785f, -0.425325f, -0.688191f },
    {  0.000000f, -0.955423f, -0.295242f },
    {  0.000000f, -1.000000f,  0.000000f },
    {  0.262866f, -0.951056f, -0.162460f },
    {  0.000000f, -0.850651f,  0.525731f },
    {  0.000000f, -0.955423f,  0.295242f },
    {  0.238856f, -0.864188f,  0.442863f },
    {  0.262866f, -0.951056f,  0.162460f },
    {  0.500000f, -0.809017f,  0.309017f },
    {  0.716567f, -0.681718f,  0.147621f },
    {  0.525731f, -0.850651f,  0.000000f },
    { -0.238856f, -0.864188f, -0.442863f },
    { -0.500000f, -0.809017f, -0.309017f },
    { -0.262866f, -0.951056f, -0.162460f },
    { -0.850651f, -0.525731f,  0.000000f },
    { -0.716567f, -0.681718f, -0.147621f },
    { -0.716567f, -0.681718f,  0.147621f },
    { -0.525731f, -0.850651f,  0.000000f },
    { -0.500000f, -0.809017f,  0.309017f },
    { -0.238856f, -0.864188f,  0.442863f },
    { -0.262866f, -0.951056f,  0.162460f },
    { -0.864188f, -0.442863f,  0.238856f },
    { -0.809017f, -0.309017f,  0.500000f },
    { -0.688191f, -0.587785f,  0.425325f },
    { -0.681718f, -0.147621f,  0.716567f },
    { -0.442863f, -0.238856f,  0.864188f },
    { -0.587785f, -0.425325f,  0.688191f },
    { -0.309017f, -0.500000f,  0.809017f },
    { -0.147621f, -0.716567f,  0.681718f },
    { -0.425325f, -0.688191f,  0.587785f },
    { -0.162460f, -0.262866f,  0.951056f },
    {  0.442863f, -0.238856f,  0.864188f },
    {  0.162460f, -0.262866f,  0.951056f },
    {  0.309017f, -0.500000f,  0.809017f },
    {  0.147621f, -0.716567f,  0.681718f },
    {  0.000000f, -0.525731f,  0.850651f },
    {  0.425325f, -0.688191f,  0.587785f },
    {  0.587785f, -0.425325f,  0.688191f },
    {  0.688191f, -0.587785f,  0.425325f },
    { -0.955423f,  0.295242f,  0.000000f },
    { -0.951056f,  0.162460f,  0.262866f },
    { -1.000000f,  0.000000f,  0.000000f },
    { -0.850651f,  0.000000f,  0.525731f },
    { -0.955423f, -0.295242f,  0.000000f },
    { -0.951056f, -0.162460f,  0.262866f },
    { -0.864188f,  0.442863f, -0.238856f },
    { -0.951056f,  0.162460f, -0.262866f },
    { -0.809017f,  0.309017f, -0.500000f },
    { -0.864188f, -0.442863f, -0.238856f },
    { -0.951056f, -0.162460f, -0.262866f },
    { -0.809017f, -0.309017f, -0.500000f },
    { -0.681718f,  0.147621f, -0.716567f },
    { -0.681718f, -0.147621f, -0.716567f },
    { -0.850651f,  0.000000f, -0.525731f },
    { -0.688191f,  0.587785f, -0.425325f },
    { -0.587785f,  0.425325f, -0.688191f },
    { -0.425325f,  0.688191f, -0.587785f },
    { -0.425325f, -0.688191f, -0.587785f },
    { -0.587785f, -0.425325f, -0.688191f },
    { -0.688191f, -0.587785f, -0.425325f }
};

#define MD2_DEFAULT_NUM_ANIMS (21)

static x_anim default_anims[MD2_DEFAULT_NUM_ANIMS] =
{
/*  first, size, fps */
    {   0,  40, 1.0f/9  }, /* STAND */
    {  40,   6, 1.0f/10 }, /* RUN */
    {  46,   8, 1.0f/10 }, /* ATTACK */
    {  54,   4, 1.0f/7  }, /* PAIN_A */
    {  58,   4, 1.0f/7  }, /* PAIN_B */
    {  62,   4, 1.0f/7  }, /* PAIN_C */
    {  66,   6, 1.0f/7  }, /* JUMP */
    {  72,  12, 1.0f/7  }, /* FLIP */
    {  84,  11, 1.0f/7  }, /* SALUTE */
    {  95,  17, 1.0f/10 }, /* FALLBACK */
    { 112,  11, 1.0f/7  }, /* WAVE */
    { 123,  12, 1.0f/6  }, /* POINT */
    { 135,  19, 1.0f/10 }, /* CROUCH_STAND */
    { 154,   6, 1.0f/7  }, /* CROUCH_WALK */
    { 160,   6, 1.0f/10 }, /* CROUCH_ATTACK */
    { 166,   6, 1.0f/7  }, /* CROUCH_PAIN */
    { 173,   5, 1.0f/5  }, /* CROUCH_DEATH */
    { 178,   6, 1.0f/7  }, /* DEATH_FALLBACK */
    { 184,   6, 1.0f/7  }, /* DEATH_FALLFORWARD */
    { 190,   8, 1.0f/7  }, /* DEATH_FALLBACKSLOW */
    { 198,   1, 1.0f/5  }, /* BOOM */
};

typedef struct
{
    int	ident;				// magic number. must be equal to "IPD2"
    int	version;			// md2 version. must be equal to 8
    int	skinwidth;			// width of the texture
    int	skinheight;			// height of the texture
    int	framesize;			// size of one frame in bytes
    int	num_skins;			// number of textures
    int	num_xyz;			// number of vertices
    int	num_st;				// number of texture coordinates
    int	num_tris;			// number of triangles
    int	num_glcmds;			// number of opengl commands
    int	num_frames;			// total number of frames
    int	ofs_skins;			// offset to skin names (64 bytes each)
    int	ofs_st;				// offset to s-t texture coordinates
    int	ofs_tris;			// offset to triangles
    int	ofs_frames;			// offset to frame data
    int	ofs_glcmds;			// offset to opengl commands
    int	ofs_end;			// offset to the end of file
} md2_header;

typedef struct
{
	u8 x, y, z;				// compressed vertex' (x, y, z) coordinates
	u8 lightnormalindex;	// index to a normal vector for the lighting
} md2_small_vertex;

typedef struct
{
	ScePspFVector3 scale;      //scale values
	ScePspFVector3 translate;  //translation vector
	char name[16];             // frame name
	md2_small_vertex verts[1]; // first vertex of this frame
} md2_frame;

int xMd2Load(xMd2* my_md2, char* filename)
{
    X_LOG("Loading MD2.", 0);
    md2_header header;
    void* frame_data;
    md2_frame* frame;
    int i, j;
    
    FILE* file = fopen(filename, "rb");
    if (!file) return 0;
    
    fread(&header, 1, sizeof(md2_header), file);
    
    X_LOG("framesize: %i, num_frames: %i, num_verts: %i, num_glcmds: %i", header.framesize, header.num_frames, header.num_xyz, header.num_glcmds);
    
    if (header.ident != MD2_IDENT || header.version != MD2_VERSION)
    {
        fclose(file);
        return 0;
    }
    
    my_md2->num_frames = header.num_frames;
    my_md2->num_verts = header.num_xyz;
    my_md2->num_glcmds = header.num_glcmds;
    
    my_md2->vertices = (ScePspFVector3*)x_malloc(my_md2->num_verts * my_md2->num_frames * sizeof(ScePspFVector3));
    my_md2->normal_indices = (u8*)x_malloc(my_md2->num_verts * my_md2->num_frames * sizeof(u8));
    my_md2->gl_commands = (int*)x_malloc(my_md2->num_glcmds * sizeof(int));
    frame_data = x_malloc(my_md2->num_frames * header.framesize);
    
    fseek(file, header.ofs_frames, SEEK_SET);
    fread(frame_data, my_md2->num_frames * header.framesize, 1, file);
    
    fseek(file, header.ofs_glcmds, SEEK_SET);
    fread(my_md2->gl_commands, my_md2->num_glcmds * sizeof(int), 1, file);
    
    fclose(file);
    
    for (i = 0; i < my_md2->num_frames; i++)
    {
        frame = (md2_frame*)((u32)frame_data + header.framesize * i);
        for (j = 0; j < my_md2->num_verts; j++)
        {
            my_md2->vertices[i*my_md2->num_verts + j].x = frame->verts[j].x * frame->scale.x + frame->translate.x;
            my_md2->vertices[i*my_md2->num_verts + j].y = frame->verts[j].y * frame->scale.y + frame->translate.y;
            my_md2->vertices[i*my_md2->num_verts + j].z = frame->verts[j].z * frame->scale.z + frame->translate.z;
            my_md2->normal_indices[i*my_md2->num_verts + j] = frame->verts[j].lightnormalindex;
        }
    }
    
    x_free(frame_data);
    X_LOG("Successfully loaded MD2.", 0);
    return 1;
}

void xMd2Free(xMd2* my_md2)
{
    X_DELETE(my_md2->vertices);
    X_DELETE(my_md2->gl_commands);
}

void xMd2AnimBind(xMd2Object* object, xMd2* my_md2, x_anim* my_anims, int num_anim, int default_anim)
{
    if (!object || !my_md2) return;
    object->md2_use = my_md2;
    if (my_anims)
    {
        object->anims_use = my_anims;
        object->num_anims = num_anim;
    }
    else
    {
        object->anims_use = default_anims;
        object->num_anims = MD2_DEFAULT_NUM_ANIMS;
    }
    xMd2AnimSet(object, default_anim, 1, 0);
    object->frame_cur = object->anims_use[object->anim].start;
    xMd2AnimUpdate(object, 0.0f);
}

void xMd2AnimSet(xMd2Object* object, int anim, int loop, int ifnot)
{
    if (!object || !object->anims_use) return;
    if (anim < 0 || anim >= object->num_anims) anim = 0;
    if (ifnot && anim == object->anim) return;
    object->anim = anim;
    if (loop) object->def_anim = object->anim;
    object->frame_next = object->anims_use[object->anim].start;
    object->time = 0.0f;
}

void xMd2AnimUpdate(xMd2Object* object, float dt)
{
    if (!object || !object->anims_use) return;
    object->time += dt;
    while (object->time > object->anims_use[object->anim].delay)
    {
        object->time -= object->anims_use[object->anim].delay;
        object->frame_cur = object->frame_next;
        object->frame_next += 1;
        if (object->frame_next >= object->anims_use[object->anim].start + object->anims_use[object->anim].size)
        {
            xMd2AnimSet(object, object->def_anim, 0, 0);
        }
    }
    object->interp = object->time / object->anims_use[object->anim].delay;
}

typedef struct
{
    float u, v;
    ScePspFVector3 normal;
    ScePspFVector3 vertex;
} md2_draw_vert;

#define md2_draw_vert_vtype (GU_TEXTURE_32BITF|GU_NORMAL_32BITF|GU_VERTEX_32BITF)

#ifdef X_MD2_HARDWARE
static md2_draw_vert md2_vert_buffer[2*MD2_MAX_VERTS];
#else
static md2_draw_vert md2_vert_buffer[MD2_MAX_VERTS];

static ScePspFVector3 md2_interp_buffer[MD2_MAX_VERTS];

static inline void interpolate_frames(xMd2Object* object)
{
    int i;
    int num = object->md2_use->num_verts;
    ScePspFVector3* cur_ptr = &object->md2_use->vertices[object->md2_use->num_verts*object->frame_cur];
    ScePspFVector3* next_ptr = &object->md2_use->vertices[object->md2_use->num_verts*object->frame_next];
    for (i = 0; i < num; i++)
    {
        x_lerp(&md2_interp_buffer[i], cur_ptr, next_ptr, object->interp);
        cur_ptr += 1;
        next_ptr += 1;
    }
}
#endif

void xMd2AnimDraw(xMd2Object* object)
{
    if (!object || !object->md2_use || !object->md2_use->gl_commands || !object->md2_use->vertices || !object->anims_use) return;
    md2_draw_vert* vertices = md2_vert_buffer;
    md2_draw_vert* vert_ptr = X_UNCACHED(vertices);
    int* glcmds = object->md2_use->gl_commands;
    int prim;
    int i, j;
    #ifndef X_MD2_HARDWARE
    interpolate_frames(object);
    #endif
    /* a zero marks the end of the commands */
    while ( (i = *(glcmds++)) )
    {
        if (i < 0)
        {
            prim = GU_TRIANGLE_FAN;
            i = -i;
        }
        else
        {
            prim = GU_TRIANGLE_STRIP;
        }
        for (j = 0; j < i; j++)
        {
            #ifdef X_MD2_HARDWARE
            vert_ptr[0].u = ((float*)glcmds)[0];
            vert_ptr[0].v = 1.0f - ((float*)glcmds)[1];
            vert_ptr[0].normal = md2_normal_lookup[object->md2_use->normal_indices[glcmds[2]]];
            vert_ptr[0].vertex = object->md2_use->vertices[object->md2_use->num_verts*object->frame_cur + glcmds[2]];
            vert_ptr[1] = vert_ptr[0];
            vert_ptr[1].vertex = object->md2_use->vertices[object->md2_use->num_verts*object->frame_next + glcmds[2]];
            vert_ptr += 2;
            #else
            vert_ptr->u = ((float*)glcmds)[0];
            vert_ptr->v = 1.0f - ((float*)glcmds)[1];
            vert_ptr->normal = md2_normal_lookup[object->md2_use->normal_indices[glcmds[2]]];
            vert_ptr->vertex = md2_interp_buffer[glcmds[2]];
            vert_ptr += 1;
            #endif
            glcmds += 3;
        }
        //sceKernelDcacheWritebackInvalidateAll();
        #ifdef X_MD2_HARDWARE
        sceGuMorphWeight(0, 1.0f - object->interp);
        sceGuMorphWeight(1, object->interp);
        sceGumDrawArray(prim, md2_draw_vert_vtype|GU_VERTICES(2)|GU_TRANSFORM_3D, i, 0, vertices);
        #else
        sceGumDrawArray(prim, md2_draw_vert_vtype|GU_TRANSFORM_3D, i, 0, vertices);
        #endif
        vertices = vert_ptr;
    }
}
