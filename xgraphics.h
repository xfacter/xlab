#ifndef __X_GRAPHICS_H__
#define __X_GRAPHICS_H__

#include "xconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/* pixel formats */
#define X_PSM_5650 (GU_PSM_5650)
#define X_PSM_5551 (GU_PSM_5551)
#define X_PSM_4444 (GU_PSM_4444)
#define X_PSM_8888 (GU_PSM_8888)

/* graphics modes */
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
#define X_WAIT_VBLANK         (1 << 29)
#define X_PSEUDO_AA           (1 << 30)
#define X_DITHER_SMOOTH       (1 << 31)

/* blend modes, thanks raphael */
#define X_BLEND_NORMAL    (0) /* alpha blending */
#define X_BLEND_ADD       (1)
#define X_BLEND_GLENZ     (2)
#define X_BLEND_ALPHA_ADD (3)
#define X_BLEND_SUB       (4)
#define X_BLEND_ALPHA_SUB (5)

/* texture filter modes */
#define X_NO_FILTER (GU_NEAREST)
#define X_BILINEAR  (GU_LINEAR)
#define X_TRILINEAR (GU_LINEAR_MIPMAP_LINEAR)

/* texture effects */
#define X_TFX_MODULATE (GU_TFX_MODULATE)
#define X_TFX_DECAL    (GU_TFX_DECAL)
#define X_TFX_BLEND    (GU_TFX_BLEND)
#define X_TFX_REPLACE  (GU_TFX_REPLACE)
#define X_TFX_ADD      (GU_TFX_ADD)

/* screen data. changing this is not recommended, may break functions */
#define X_SCREEN_WIDTH (480)
#define X_SCREEN_HEIGHT (272)
#define X_BUFFER_WIDTH (512)

/* color macros */
#define X_COLOR_5650(r,g,b)   (((r)<<0)|((g)<<5)|((b)<<11))
#define X_COLOR_5551(r,g,b,a) (((r)<<0)|((g)<<5)|((b)<<10)|((a)<<15)
#define X_COLOR_4444(r,g,b,a) (((r)<<0)|((g)<<4)|((b)<<8)|((a)<<12))
#define X_COLOR_8888(r,g,b,a) (((r)<<0)|((g)<<8)|((b)<<16)|((a)<<24))
#define X_COLOR_5650_32BITF(r,g,b)   (X_COLOR_5650((u8)(31.f*(r)), (u8)(63.f*(g)), (u8)(31.f*(b))))
#define X_COLOR_5551_32BITF(r,g,b,a) (X_COLOR_5551((u8)(31.f*(r)), (u8)(31.f*(g)), (u8)(31.f*(b)), (u8)(1.f*(a))))
#define X_COLOR_4444_32BITF(r,g,b,a) (X_COLOR_4444((u8)(15.f*(r)), (u8)(15.f*(g)), (u8)(15.f*(b)), (u8)(15.f*(a))))
#define X_COLOR_8888_32BITF(r,g,b,a) (X_COLOR_8888((u8)(255.f*(r)),(u8)(255.f*(g)),(u8)(255*(b)),(u8)(255.f*(a))))

void xGuInit(int psm, u32 list_size);

void xGuEnd();

int xGuFrameEnd();

void xGuClear(u32 color);

void xGuPerspective(float fovy);

int xGuSetOrtho();

int xGuSetPerspective();

void xGuEnable(int states);

void xGuDisable(int states);

void xGuRenderTarget(int psm, int width, int height, int tbw, void* tbp);

void xGuRenderReset();

void xGuBlend(int mode);

void xGuTexFilter(int filter);

void xGuTexMode(int tfx, int alpha, int repeat);

void xGuClutFunc(int cpsm, int entries, void* cbp);

void xGuTexFunc(int swizzle, float u_scale, float v_scale, int tpsm, int width, int height, int tbw, void* tbp);

void xGumLoadIdentity();

void xGumTranslate(float x, float y, float z);

void xGumRotateX(float angle);

void xGumRotateY(float angle);

void xGumRotateZ(float angle);

void xGumScale(float x, float y, float z);

void xGumCamUp(float x, float y, float z);

void xGumCamOrient(float px, float py, float pz, float dx, float dy, float dz);

u32 xGuMemAvail();

void xGuSaveStates();
void xGuLoadStates();
void* xGuDrawPtr(int uncached, int abs);
void* xGuDispPtr(int uncached, int abs);
void* xGuDepthPtr(int uncached, int abs);
void* xGuStridePtr(int uncached, int abs);
void xGuSetDebugTex();
void xGumDrawUnitTexQuad();
void xGuDrawTex(int x, int y, int w, int h, int sx, int sy, int sw, int sh);
void xGuTexScaleToDest(int psm, int sw, int sh, int sbw, void* src, int dw, int dh, int dbw, void* dest);
void xGuDrawOverlay(u32 color);

#ifdef __cplusplus
}
#endif

#endif
