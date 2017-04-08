#ifndef __X_GRAPHICS_H__
#define __X_GRAPHICS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Macros */
#define X_PIXEL_BYTES (4)
#define X_BUFFER_WIDTH (512)
#define X_SCREEN_WIDTH (480)
#define X_SCREEN_HEIGHT (272)
#define X_FRAME_BUFFER_SIZE (X_PIXEL_BYTES*X_BUFFER_WIDTH*X_SCREEN_HEIGHT)

#define X_ALPHA_TEST          (1 << GU_ALPHA_TEST)
#define X_DEPTH_TEST          (1 << GU_DEPTH_TEST)
#define X_SCISSOR_TEST        (1 << GU_SCISSOR_TEST)
#define X_STENCIL_TEST        (1 << GU_STENCIL_TEST)
#define X_BLEND               (1 << GU_BLEND)
#define X_CULL_FACE           (1 << GU_CULL_FACE)
#define X_DITHER              (1 << GU_DITHER)
#define X_FOG                 (1 << GU_FOG)
#define X_CLIP_PLANES         (1 << GU_CLIP_PLANES)
#define X_TEXTURE_2D          (1 << GU_TEXTURE_2D)
#define X_LIGHTING            (1 << GU_LIGHTING)
#define X_LIGHT0              (1 << GU_LIGHT0)
#define X_LIGHT1              (1 << GU_LIGHT1)
#define X_LIGHT2              (1 << GU_LIGHT2)
#define X_LIGHT3              (1 << GU_LIGHT3)
#define X_LINE_SMOOTH         (1 << GU_LINE_SMOOTH)
#define X_PATCH_CULL_FACE     (1 << GU_PATCH_CULL_FACE)
#define X_COLOR_TEST          (1 << GU_COLOR_TEST)
#define X_COLOR_LOGIC_OP      (1 << GU_COLOR_LOGIC_OP)
#define X_FACE_NORMAL_REVERSE (1 << GU_FACE_NORMAL_REVERSE)
#define X_PATCH_FACE          (1 << GU_PATCH_FACE)
#define X_FRAGMENT_2X         (1 << GU_FRAGMENT_2X)

void xGuInit(unsigned int list_size, int states, unsigned int bgcolor);

void xGuEnd();

void xGuFrameStart();

void xGuFrameEnd();

void xGuSaveStates();

void xGuLoadStates();

int xGuGetStatus(int states);

void xGuEnableStates(int states);

void xGuDisableStates(int states);

void xGuBlendDefault();

void xGuTexFilter(int aa);

void xGuPerspective(float fovy, float near, float far);

void xGuSwapBuffers(int wait_vblank);

void xGuPseudoAntiAliase(float fov_change, int dither);

unsigned int xGuPixelColor(int x, int y);

void xGumCallList(const void* list);

void xGumMove(float x, float y, float z);

void xGumSetPos(float x, float y, float z);

void xGumScaleMulti(float x, float y, float z);

void xGumScaleSingle(float scale);

void xGumSetCameraPos(float x, float y, float z);

void xGumFrustumCullStart(float x_low, float y_low, float z_low, float x_high, float y_high, float z_high);

void xGumFrustumCullEnd();

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
    unsigned int color;
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
    unsigned int color;
    float x, y, z;
} TCVertexF;

#define TCVertexF_vtype (GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF)

typedef struct {
    unsigned int color;
    float nx, ny, nz;
    float x, y, z;
} CNVertexF;

#define CNVertexF_vtype (GU_COLOR_8888|GU_NORMAL_32BITF|GU_VERTEX_32BITF)

typedef struct {
    s16 x, y, z;
} Vertex2D;

#define Vertex2D_vtype (GU_VERTEX_16BIT)

typedef struct {
    unsigned int color;
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
    unsigned int color;
    s16 x, y, z;
} TCVertex2D;

#define TCVertex2D_vtype (GU_TEXTURE_16BIT|GU_COLOR_8888|GU_VERTEX_16BIT)

typedef struct {
    s8 u, v;
    float x, y, z;
} GenVert;

#define xGenericVert_vtype (GU_TEXTURE_8BIT|GU_VERTEX_32BITF)
#define xGenericVert_prim (GU_TRIANGLE_FAN)
#define xGenericVert_count (4)

extern GenVert xGenericVert[4];

#ifdef __cplusplus
}
#endif

#endif
