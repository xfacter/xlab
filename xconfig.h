#ifndef __X_CONFIG_H__
#define __X_CONFIG_H__

#include <stdio.h>
#include <pspkernel.h>
#include <psptypes.h>

/* TODO:
    * 3D text (xtext.h) (fix)
    * custom draw function (allowing user to enable special options like clipping, cel shading, and .. other stuff)
    * ?
**/

#define X_DEBUG

#define X_LIB_USER
//#define X_LIB_KERNEL

//#define X_MODULE_WIFI
//#define X_MODULE_MP3

#define X_DLIST_SINGLE
//#define X_DLIST_DOUBLE

#define X_DEPTH_BUFFER
//#define X_NO_DEPTH_BUFFER

#define X_TEX_TGA
//#define X_TEX_PNG
//#define X_TEX_BMP

#define X_PARTICLE_COLOR
//#define X_PARTICLE_NO_COLOR

#define X_MD2_SOFTWARE
//#define X_MD2_HARDWARE

typedef struct {
    int start;
    int size;
    float delay;
} x_anim;

/* vertex structures */

typedef struct {
    float x, y, z;
} VertexF;

#define VertexF_vtype (GU_VERTEX_32BITF)

typedef struct {
    float u, v;
    float x, y, z;
} TVertexF;

#define TVertexF_vtype (GU_TEXTURE_32BITF|GU_VERTEX_32BITF)

typedef struct {
    u32 color;
    float x, y, z;
} CVertexF;

#define CVertexF_vtype (GU_COLOR_8888|GU_VERTEX_32BITF)

typedef struct {
    float nx, ny, nz;
    float x, y, z;
} NVertexF;

#define NVertexF_vtype (GU_NORMAL_32BITF|GU_VERTEX_32BITF)

typedef struct {
    float u, v;
    float nx, ny, nz;
    float x, y, z;
} TNVertexF;

#define TNVertexF_vtype (GU_COLOR_8888|GU_NORMAL_32BITF|GU_VERTEX_32BITF)

typedef struct {
    float u, v;
    u32 color;
    float x, y, z;
} TCVertexF;

#define TCVertexF_vtype (GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF)

typedef struct {
    u32 color;
    float nx, ny, nz;
    float x, y, z;
} CNVertexF;

#define CNVertexF_vtype (GU_COLOR_8888|GU_NORMAL_32BITF|GU_VERTEX_32BITF)

typedef struct {
    s16 x, y, z;
} Vertex2D;

#define Vertex2D_vtype (GU_VERTEX_16BIT)

typedef struct {
    u32 color;
    s16 x, y, z;
} CVertex2D;

#define CVertex2D_vtype (GU_COLOR_8888|GU_VERTEX_16BIT)

typedef struct {
    s16 u, v;
    s16 x, y, z;
} TVertex2D;

#define TVertex2D_vtype (GU_TEXTURE_16BIT|GU_VERTEX_16BIT)

typedef struct {
    s16 u, v;
    u32 color;
    s16 x, y, z;
} TCVertex2D;

#define TCVertex2D_vtype (GU_TEXTURE_16BIT|GU_COLOR_8888|GU_VERTEX_16BIT)

#endif
