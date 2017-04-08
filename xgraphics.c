#include <pspgu.h>
#include <pspgum.h>
#include <pspdisplay.h>
#include "xmem.h"
#include "xmath.h"

#include "xgraphics.h"

#define PERSPECT_NEAR (0.5f)
#define PERSPECT_FAR (1000.0f)

#define ORTHO_LEFT (0.0f)
#define ORTHO_RIGHT (480.0f)
#define ORTHO_BOTTOM (272.0f)
#define ORTHO_TOP (0.0f)
#define ORTHO_NEAR (-1000.0f)
#define ORTHO_FAR (1000.0f)

#define X_CUSTOM_STATES (X_WAIT_VBLANK|X_PSEUDO_AA|X_DITHER_SMOOTH)

#define X_FLAGS_ENABLE(x) (x_flags |= (x))
#define X_FLAGS_DISABLE(x) (x_flags &= ~(x))

#define X_WHICH_MATRIX (x_flags & X_PSEUDO_AA ? x_which_buf : 0)
#define X_WHICH_DITHER (x_flags & X_DITHER_SMOOTH ? x_which_buf : 0)

static u32 x_dlist_size = 0;
static void* x_dlist0;
#ifdef X_DLIST_DOUBLE
static void* x_dlist1;
static int x_which_list = 0;
static void* x_call_list;
#endif

static int x_which_buf = 0;
static void* x_draw_buf[2] = {0, 0};
static void* x_depth_buf = 0;
static int x_flags = 0;
static int x_inortho = 0;
int x_buf_psm = 0;
int x_buf_pixel_size = 0;

static ScePspFMatrix4 x_perspect_mtx[2];
static ScePspFMatrix4 x_ortho_mtx[2];

static int x_saved_states;

static int x_dither_matrix[2][16] =
{
    { 0, 8, 0, 8,
      8, 0, 8, 0,
      0, 8, 0, 8,
      8, 0, 8, 0 },
    { 8, 8, 8, 8,
      0, 8, 0, 8,
      8, 8, 8, 8,
      0, 8, 0, 8 }
};

void xGuInit(int psm, u32 list_size)
{
    switch (psm)
    {
        case GU_PSM_5650:
        case GU_PSM_5551:
        case GU_PSM_4444:
            x_buf_pixel_size = 2;
            break;
        default:
            x_buf_pixel_size = 4;
            psm = GU_PSM_8888;
            break;
    }
    x_buf_psm = psm;
    
    if (x_dlist0) xGuEnd();
    
    x_dlist_size = list_size;
    x_dlist0 = x_malloc(x_dlist_size);
    #ifdef X_DLIST_DOUBLE
    x_dlist1 = x_malloc(x_dlist_size);
    x_which_list = 0;
    x_call_list = x_malloc(32);
    #endif
    
    sceGuInit();
    
    #ifndef X_DLIST_DOUBLE
    sceGuStart(GU_DIRECT, x_dlist0);
    #else
    sceGuStart(GU_CALL, (x_which_list == 0 ? x_dlist0 : x_dlist1));
    #endif

    x_which_buf = 0;
    x_draw_buf[0] = X_VREL(x_valloc(x_buf_pixel_size*X_BUFFER_WIDTH*X_SCREEN_HEIGHT));
    x_draw_buf[1] = X_VREL(x_valloc(x_buf_pixel_size*X_BUFFER_WIDTH*X_SCREEN_HEIGHT));
    sceGuDrawBuffer(x_buf_psm, x_draw_buf[0], X_BUFFER_WIDTH);
    sceGuDispBuffer(X_SCREEN_WIDTH, X_SCREEN_HEIGHT, x_draw_buf[1], X_BUFFER_WIDTH);
    
    sceGuOffset(2048 - (X_SCREEN_WIDTH/2), 2048 - (X_SCREEN_HEIGHT/2));
    sceGuViewport(2048, 2048, X_SCREEN_WIDTH, X_SCREEN_HEIGHT);
    sceGuScissor(0, 0, X_SCREEN_WIDTH, X_SCREEN_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);

    #ifndef X_NO_DEPTH_BUFFER
    x_depth_buf = X_VREL(x_valloc(2*X_BUFFER_WIDTH*X_SCREEN_HEIGHT));
    sceGuDepthBuffer(x_depth_buf, X_BUFFER_WIDTH);
    sceGuDepthRange(65535, 0);
    sceGuDepthFunc(GU_GEQUAL);
    sceGuClearDepth(0x0000);
    sceGuEnable(GU_DEPTH_TEST);
    sceGuDepthMask(GU_FALSE);
    sceGuEnable(GU_CLIP_PLANES);
    sceGuEnable(GU_CULL_FACE);
    sceGuFrontFace(GU_CW);
    #else
    sceGuDisable(GU_DEPTH_TEST);
    sceGuDepthMask(GU_TRUE);
    sceGuDisable(GU_CLIP_PLANES);
    sceGuDisable(GU_CULL_FACE);
    #endif
    
    sceGuShadeModel(GU_SMOOTH);
    xGuBlend(X_BLEND_NORMAL);
    
    gumLoadIdentity(&x_ortho_mtx[0]);
    gumOrtho(&x_ortho_mtx[0], ORTHO_LEFT, ORTHO_RIGHT, ORTHO_BOTTOM, ORTHO_TOP, ORTHO_NEAR, ORTHO_FAR);
    gumLoadIdentity(&x_ortho_mtx[1]);
    ScePspFVector3 displace = {-1.0f/X_SCREEN_WIDTH, 1.0f/X_SCREEN_HEIGHT, 0.0f};
    gumTranslate(&x_ortho_mtx[1], &displace);
    gumMultMatrix(&x_ortho_mtx[1], &x_ortho_mtx[1], &x_ortho_mtx[0]);
    xGuPerspective(75.0f);
    sceGumMatrixMode(GU_PROJECTION);
    sceGumLoadMatrix(&x_perspect_mtx[X_WHICH_MATRIX]);
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
    sceGuFrontFace(GU_CW);
    x_inortho = 0;
    
    sceGuDisplay(1);
}

void xGuEnd()
{
    if (!x_dlist0) return;
    x_free(x_dlist0);
    x_dlist0 = 0;
    #ifdef X_DLIST_DOUBLE
    x_free(x_dlist1);
    x_dlist1 = 0;
    x_free(x_call_list);
    x_call_list = 0;
    #endif
    X_VFREE(x_draw_buf[0]);
    x_draw_buf[0] = 0;
    X_VFREE(x_draw_buf[1]);
    x_draw_buf[1] = 0;
    if (x_depth_buf)
    {
        X_VFREE(x_depth_buf);
        x_depth_buf = 0;
    }
    sceGuTerm();
}

int xGuFrameEnd()
{
    /* end current frame */
    int size = sceGuFinish();
    sceGuSync(0, 0);
    if (x_flags & X_WAIT_VBLANK || x_flags & X_PSEUDO_AA)
    {
        sceDisplayWaitVblankStart();
    }
    x_which_buf = (sceGuSwapBuffers() == x_draw_buf[0] ? 0 : 1);
    #ifdef X_DLIST_DOUBLE
    sceGuStart(GU_DIRECT, x_call_list);
    sceGuCallList((x_which_list == 0 ? x_dlist0 : x_dlist1));
    sceGuFinish();
    x_which_list ^= 1;
    #endif
    if (x_flags & X_PSEUDO_AA)
    {
        sceGumMatrixMode(GU_PROJECTION);
        if (!x_inortho) sceGumLoadMatrix(&x_perspect_mtx[X_WHICH_MATRIX]);
        else sceGumLoadMatrix(&x_ortho_mtx[X_WHICH_MATRIX]);
        sceGumMatrixMode(GU_MODEL);
        x_inortho = 0;
    }
    if (x_flags & X_DITHER_SMOOTH)
    {
        sceGuSetDither((ScePspIMatrix4*)(&x_dither_matrix[X_WHICH_DITHER]));
    }
    
    /* begin next frame */
    #ifndef X_DLIST_DOUBLE
    sceGuStart(GU_DIRECT, x_dlist0);
    #else
    sceGuStart(GU_CALL, (x_which_list == 0 ? x_dlist0 : x_dlist1));
    #endif
    #ifndef X_NO_DEPTH_BUFFER
    sceGuClear(GU_DEPTH_BUFFER_BIT);
    #endif
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
    return size;
}

void xGuClear(u32 color)
{
    sceGuClearColor(color);
    sceGuClear(GU_COLOR_BUFFER_BIT|GU_FAST_CLEAR_BIT);
}

void xGuPerspective(float fovy)
{
    gumLoadIdentity(&x_perspect_mtx[0]);
    gumPerspective(&x_perspect_mtx[0], fovy, 16.0f/9.0f, PERSPECT_NEAR, PERSPECT_FAR);
    gumLoadIdentity(&x_perspect_mtx[1]);
    ScePspFVector3 displace = {-1.0f/X_SCREEN_WIDTH, 1.0f/X_SCREEN_HEIGHT, 0.0f};
    gumTranslate(&x_perspect_mtx[1], &displace);
    gumMultMatrix(&x_perspect_mtx[1], &x_perspect_mtx[1], &x_perspect_mtx[0]);
}

int xGuSetOrtho()
{
    if (x_inortho) return 0;
    sceGumMatrixMode(GU_PROJECTION);
    sceGumLoadMatrix(&x_ortho_mtx[X_WHICH_MATRIX]);
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
    /* hm */
    sceGuFrontFace(GU_CCW);
    x_inortho = 1;
    return 1;
}

int xGuSetPerspective()
{
    if (!x_inortho) return 0;
    sceGumMatrixMode(GU_PROJECTION);
    sceGumLoadMatrix(&x_perspect_mtx[X_WHICH_MATRIX]);
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
    /* hm */
    sceGuFrontFace(GU_CW);
    x_inortho = 0;
    return 1;
}

void xGuEnable(int states)
{
    if (states & X_CUSTOM_STATES) X_FLAGS_ENABLE(states & X_CUSTOM_STATES);
    if (states & X_DITHER_SMOOTH) states |= X_DITHER;
    sceGuSetAllStatus(sceGuGetAllStatus() | states);
}

void xGuDisable(int states)
{
    if (states & X_CUSTOM_STATES) X_FLAGS_DISABLE(states & X_CUSTOM_STATES);
    if (states & X_DITHER_SMOOTH) states |= X_DITHER;
    sceGuSetAllStatus(sceGuGetAllStatus() & ~states);
}

void xGuRenderTarget(int psm, int width, int height, int tbw, void* tbp)
{
    sceGuDrawBufferList(psm, tbp, tbw);
    sceGuOffset(2048 - (width/2), 2048 - (height/2));
    sceGuViewport(2048, 2048, width, height);
    sceGuScissor(0, 0, width, height);
}

void xGuRenderReset()
{
    sceGuDrawBufferList(x_buf_psm, x_draw_buf[x_which_buf], X_BUFFER_WIDTH);
    sceGuOffset(2048 - (X_SCREEN_WIDTH/2), 2048 - (X_SCREEN_HEIGHT/2));
    sceGuViewport(2048, 2048, X_SCREEN_WIDTH, X_SCREEN_HEIGHT);
    sceGuScissor(0, 0, X_SCREEN_WIDTH, X_SCREEN_HEIGHT);
}

void xGuBlend(int mode)
{
    switch (mode)
    {
        case X_BLEND_ADD:
            sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, 0xFFFFFFFF, 0xFFFFFFFF);
            break;
        case X_BLEND_GLENZ:
            sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, 0x7F7F7F7F, 0x7F7F7F7F);
            break;
        case X_BLEND_ALPHA_ADD:
            sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_FIX, 0, 0xFFFFFFFF);
            break;
        case X_BLEND_SUB:
            sceGuBlendFunc(GU_REVERSE_SUBTRACT, GU_FIX, GU_FIX, 0xFFFFFFFF, 0xFFFFFFFF);
            break;
        case X_BLEND_ALPHA_SUB:
            sceGuBlendFunc(GU_REVERSE_SUBTRACT, GU_SRC_ALPHA, GU_FIX, 0, 0xFFFFFFFF);
            break;
        default:
            sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
            break;
    }
}

inline void xGuTexFilter(int filter)
{
    sceGuTexFilter(filter, filter);
}

inline void xGuTexMode(int tfx, int alpha, int repeat)
{
    sceGuTexFunc(tfx, (alpha ? GU_TCC_RGBA : GU_TCC_RGB));
    sceGuTexWrap((repeat ? GU_REPEAT : GU_CLAMP), (repeat ? GU_REPEAT : GU_CLAMP));
}

inline void xGuClutFunc(int cpsm, int entries, void* cbp)
{
    sceGuClutMode(cpsm, 0, 0xffffffff, 0);
    sceGuClutLoad(entries >> 3, cbp);
}

inline void xGuTexFunc(int swizzle, float u_scale, float v_scale, int tpsm, int width, int height, int tbw, void* tbp)
{
    sceGuTexMode(tpsm, 0, 0, swizzle);
    sceGuTexScale(u_scale, v_scale);
    sceGuTexImage(0, width, height, tbw, tbp);
}

inline void xGumLoadIdentity()
{
    sceGumLoadIdentity();
}

inline void xGumTranslate(float x, float y, float z)
{
    ScePspFVector3 trans = {x,y,z};
    sceGumTranslate(&trans);
}

inline void xGumRotateX(float angle)
{
    sceGumRotateX(angle);
}

inline void xGumRotateY(float angle)
{
    sceGumRotateY(angle);
}

inline void xGumRotateZ(float angle)
{
    sceGumRotateZ(angle);
}

inline void xGumScale(float x, float y, float z)
{
    ScePspFVector3 scale = {x,y,z};
    sceGumScale(&scale);
}

static ScePspFVector3 x_up = {0.0f, 0.0f, 1.0f};

inline void xGumCamUp(float x, float y, float z)
{
    x_up.x = x;
    x_up.y = y;
    x_up.z = z;
}

void xGumCamOrient(float px, float py, float pz, float dx, float dy, float dz)
{
    ScePspFVector3 eye = {px, py, pz};
    ScePspFVector3 lookat = {px + dx, py + dy, pz + dz};
    sceGumLookAt(&eye, &lookat, &x_up);
}

inline u32 xGuMemAvail()
{
    return (x_dlist_size - sceGuCheckList());
}

inline void xGuSaveStates()
{
    x_saved_states = x_flags|sceGuGetAllStatus();
}
inline void xGuLoadStates()
{
    sceGuSetAllStatus(x_saved_states & ~X_CUSTOM_STATES);
    xGuEnable(x_saved_states & X_CUSTOM_STATES);
}

inline void* xGuDrawPtr(int uncached, int abs)
{
    u32 ptr = (u32)x_draw_buf[x_which_buf];
    if (uncached) ptr |= X_MEM_NO_CACHE;
    if (abs) ptr |= X_MEM_VRAM;
    return (void*)ptr;
}

inline void* xGuDispPtr(int uncached, int abs)
{
    u32 ptr = (u32)x_draw_buf[x_which_buf^1];
    if (uncached) ptr |= X_MEM_NO_CACHE;
    if (abs) ptr |= X_MEM_VRAM;
    return (void*)ptr;
}

inline void* xGuDepthPtr(int uncached, int abs)
{
    u32 ptr = (u32)x_depth_buf;
    if (uncached) ptr |= X_MEM_NO_CACHE;
    if (abs) ptr |= X_MEM_VRAM;
    return (void*)ptr;
}

inline void* xGuStridePtr(int uncached, int abs)
{
    u32 ptr = (u32)x_draw_buf[0] + X_SCREEN_WIDTH*x_buf_pixel_size;
    if (uncached) ptr |= X_MEM_NO_CACHE;
    if (abs) ptr |= X_MEM_VRAM;
    return (void*)ptr;
}

#define TEX_DEBUG_PSM (GU_PSM_8888)
#define TEX_DEBUG_SCALE (5.0f)
#define TEX_DEBUG_WIDTH (4)
#define TEX_DEBUG_HEIGHT (4)

static u32 __attribute__((aligned(16))) debug_texture[16] = {
    0xffff00ff, 0xff000000, 0xffff00ff, 0xff000000,
    0xff000000, 0xffff00ff, 0xff000000, 0xffff00ff,
    0xffff00ff, 0xff000000, 0xffff00ff, 0xff000000,
    0xff000000, 0xffff00ff, 0xff000000, 0xffff00ff
};

void xGuSetDebugTex()
{
    /* swizzle dat shit? */
    sceGuTexMode(TEX_DEBUG_PSM, 0, 0, 0);
    sceGuTexScale(TEX_DEBUG_SCALE, TEX_DEBUG_SCALE);
    sceGuTexImage(0, TEX_DEBUG_WIDTH, TEX_DEBUG_HEIGHT, TEX_DEBUG_WIDTH, debug_texture);
}

typedef struct {
    s8 u, v;
    float x, y, z;
} GenVert;

static GenVert __attribute__((aligned(16))) xGenericVert[4] = {
    {0,     0, -0.5f, -0.5f,  0.0f},
    {0,   127, -0.5f,  0.5f,  0.0f},
    {127, 127,  0.5f,  0.5f,  0.0f},
    {127,   0,  0.5f, -0.5f,  0.0f}
};

#define xGenericVert_vtype (GU_TEXTURE_8BIT|GU_VERTEX_32BITF)
#define xGenericVert_prim (GU_TRIANGLE_FAN)
#define xGenericVert_count (4)

inline void xGumDrawUnitTexQuad()
{
    sceGumDrawArray(xGenericVert_prim, xGenericVert_vtype|GU_TRANSFORM_3D, xGenericVert_count, 0, xGenericVert);
}

#define TEX_SLICE (64)

void xGuDrawTex(int x, int y, int w, int h, int sx, int sy, int sw, int sh)
{
    float cur_u = (float)sx;
    float ustep = (float)TEX_SLICE*sw/w;
    int slice_width = 0;
    float tex_step = 0;
    int i;
    for (i = x; i < x + w; i += TEX_SLICE)
    {
        slice_width = (i + TEX_SLICE > x + w ? x + w - i : TEX_SLICE);
        tex_step = (cur_u + ustep > sx + sw ? sx + sw - cur_u : ustep);
        
        TVertex2D* vertices = (TVertex2D*)sceGuGetMemory(2*sizeof(TVertex2D));
        vertices[0].u = (s16)cur_u;
        vertices[0].v = sy + sh;
        vertices[0].x = i;
        vertices[0].y = y;
        vertices[0].z = 0;
        cur_u += tex_step;
        vertices[1].u = (s16)cur_u;
        vertices[1].v = sy;
        vertices[1].x = i + slice_width;
        vertices[1].y = y + h;
        vertices[1].z = 0;
        sceGuDrawArray(GU_SPRITES, TVertex2D_vtype|GU_TRANSFORM_2D, 2, 0, vertices);
    }
}

/* wont work for images that arent the same psm as framebuffer and outside of vram - needs fixing */
void xGuTexScaleToDest(int psm, int sw, int sh, int sbw, void* src, int dw, int dh, int dbw, void* dest)
{
    if ((psm != GU_PSM_4444 && psm != GU_PSM_5551 && psm != GU_PSM_5650 && psm != GU_PSM_8888) ||
        (dw > 256 || dh > 256)) return;
    xGuSaveStates();
    xGuDisable(X_CUSTOM_STATES);
    sceGuEnable(GU_TEXTURE_2D);
    sceGuDisable(GU_BLEND);
    sceGuDisable(GU_DEPTH_TEST);
    xGuTexFilter(X_BILINEAR);
    xGuTexMode(X_TFX_REPLACE, 1, 0);
    xGuTexFunc(0, 1.0f, 1.0f, psm, x_next_pow2(sw), x_next_pow2(sh), sbw, src);
    //int in_vram = ((u32)dest >= X_MEM_VRAM && (u32)dest < X_MEM_VRAM + X_MEM_VRAM_SIZE);
    //if (in_vram) xGuRenderTarget(psm, dw, dh, dbw, dest);
    //else sceGuDrawBufferList(psm, xGuDrawPtr(0, 0), 256); //xGuRenderTarget(psm, dw, dh, dbw, xGuDrawPtr(0, 0));
    xGuRenderTarget(psm, 256, 256, 256, xGuDrawPtr(0, 0));
    TVertex2D* vertices = (TVertex2D*)sceGuGetMemory(2*sizeof(TVertex2D));
    vertices[0].u = 0;
    vertices[0].v = 0;
    vertices[0].x = 0;
    vertices[0].y = 0;
    vertices[0].z = 0;
    vertices[1].u = sw;
    vertices[1].v = sh;
    vertices[1].x = dw;
    vertices[1].y = dh;
    vertices[1].z = 0;
    sceGuDrawArray(GU_SPRITES, TVertex2D_vtype|GU_TRANSFORM_2D, 2, 0, vertices);
    //if (in_vram) xGuRenderReset();
    //else sceGuDrawBufferList(X_BUFFER_PSM, xGuDrawPtr(0, 0), 512);
    xGuRenderReset();
    sceGuSync(0, 2);
    xGuTexFilter(X_NO_FILTER);
    //if (!in_vram)
    //{
        sceGuCopyImage(psm, 0, 0, dw, dh, 256, xGuDrawPtr(0, 1), 0, 0, dbw, dest);
        sceGuTexSync();
    //}
    xGuLoadStates();
}

void xGuDrawOverlay(u32 color)
{
    CVertex2D* quad = (CVertex2D*)sceGuGetMemory(4*sizeof(CVertex2D));
    quad[0].color = color;
    quad[0].x = 0;
    quad[0].y = 0;
    quad[0].z = 0;
    quad[1].color = color;
    quad[1].x = X_SCREEN_WIDTH;
    quad[1].y = 0;
    quad[1].z = 0;
    quad[2].color = color;
    quad[2].x = X_SCREEN_WIDTH;
    quad[2].y = X_SCREEN_HEIGHT;
    quad[2].z = 0;
    quad[3].color = color;
    quad[3].x = 0;
    quad[3].y = X_SCREEN_HEIGHT;
    quad[3].z = 0;
    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuDisable(GU_TEXTURE_2D);
    sceGuDrawArray(GU_TRIANGLE_FAN, CVertex2D_vtype|GU_TRANSFORM_2D, 4, 0, quad);
    xGuLoadStates();
}
