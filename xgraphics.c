#include <stdlib.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspdisplay.h>
#include <psptypes.h>
#include <vram.h>

#include "xgraphics.h"

#define ADDR_PIXEL_DATA ((void*)((u32)fbp0|0x44000000))

static u8* xGu_displaylist;

void xGuInit(unsigned int list_size, int states, unsigned int bgcolor)
{
    if (xGu_displaylist) return;
    
    xGu_displaylist = (u8*)malloc(list_size*sizeof(u8));
    
    sceGuInit();
    
    xGuFrameStart();
    
    void* draw_buf = vrelptr(valloc(X_FRAME_BUFFER_SIZE));
    void* disp_buf = vrelptr(valloc(X_FRAME_BUFFER_SIZE));
    void* depth_buf = vrelptr(valloc(X_FRAME_BUFFER_SIZE/2));
    sceGuDrawBuffer(GU_PSM_8888, draw_buf, X_BUFFER_WIDTH);
    sceGuDispBuffer(X_SCREEN_WIDTH, X_SCREEN_HEIGHT, disp_buf, X_BUFFER_WIDTH);
    sceGuDepthBuffer(depth_buf, X_BUFFER_WIDTH);
    
    sceGuOffset(2048 - (X_SCREEN_WIDTH/2), 2048 - (X_SCREEN_HEIGHT/2));
    sceGuViewport(2048, 2048, X_SCREEN_WIDTH, X_SCREEN_HEIGHT);
    sceGuDepthRange(65535, 0);
    
    sceGuScissor(0, 0, X_SCREEN_WIDTH, X_SCREEN_HEIGHT);
    sceGuDepthFunc(GU_GEQUAL);
    sceGuFrontFace(GU_CW);
    sceGuShadeModel(GU_SMOOTH);
    
    xGuEnableStates(states);
    
    xGuBlendDefault();
    
    sceGuClearColor(bgcolor);
    sceGuClearDepth(0);
    sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
    
    xGuFrameEnd();
    xGuSwapBuffers(1);
    
    sceGuDisplay(1);
}

void xGuEnd()
{
    if (!xGu_displaylist) return;
    free(xGu_displaylist);
    sceGuTerm();
}

static u8 xGu_inframe = 0;

void xGuFrameStart()
{
    if (xGu_inframe) return;
    sceGuStart(GU_DIRECT, xGu_displaylist);
    sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
    xGu_inframe = 1;
}

void xGuFrameEnd()
{
    if (!xGu_inframe) return;
    sceGuFinish();
    sceGuSync(0, 0);
    xGu_inframe = 0;
}

static int gu_states;

void xGuSaveStates()
{
    gu_states = sceGuGetAllStatus();
}
void xGuLoadStates()
{
    sceGuSetAllStatus(gu_states);
}

int xGuGetStatus(int states)
{
    return ((sceGuGetAllStatus() & states) == states);
}

void xGuEnableStates(int states)
{
    sceGuSetAllStatus(sceGuGetAllStatus() | states);
}

void xGuDisableStates(int states)
{
    sceGuSetAllStatus(sceGuGetAllStatus() & ~states);
}

void xGuTexFilter(int aa)
{
    aa = (aa ? GU_LINEAR : GU_NEAREST);
    sceGuTexFilter(aa, aa);
}

void xGuBlendDefault()
{
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
}

static float gu_fovy, gu_near, gu_far;

void xGuPerspective(float fovy, float near, float far)
{
    gu_fovy = fovy;
    gu_near = near;
    gu_far = far;
    
    sceGumMatrixMode(GU_PROJECTION);
    sceGumLoadIdentity();
    sceGumPerspective(fovy, 16.0f/9.0f, near, far);
    sceGumMatrixMode(GU_MODEL);
}

static void* fbp0;

void xGuSwapBuffers(int wait_vblank)
{
    if (wait_vblank) sceDisplayWaitVblankStart();
    fbp0 = sceGuSwapBuffers();
}

int dither_matrix[2][16] =
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

void xGuPseudoAntiAliase(float fov_change, int dither)
{
    sceGumMatrixMode(GU_PROJECTION);
    sceGumLoadIdentity();
    sceGumPerspective(gu_fovy + (fbp0?1:0)*fov_change, 16.0f/9.0f, gu_near, gu_far);
    if (dither) sceGuSetDither((ScePspIMatrix4*)dither_matrix[(fbp0?1:0)]);
}

unsigned int xGuPixelColor(int x, int y)
{
    if (x < 0 || y < 0 || x > X_BUFFER_WIDTH || y > X_SCREEN_HEIGHT) return 0;
    unsigned int* color = (unsigned int*)ADDR_PIXEL_DATA;
    return (0xff << 24 | color[y*X_BUFFER_WIDTH + x]);
}

void xGumCallList(const void* list)
{
    sceGumUpdateMatrix();
    sceGuCallList(list);
}

void xGumMove(float x, float y, float z)
{
    ScePspFVector3 move = {x, y, z};
    sceGumTranslate(&move);
}

void xGumSetPos(float x, float y, float z)
{
    sceGumLoadIdentity();
    xGumMove(x, y, z);
}

void xGumSetCameraPos(float x, float y, float z)
{
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();
    xGumSetPos(-x, -y, -z);
    sceGumMatrixMode(GU_MODEL);
}

void xGumScaleMulti(float x, float y, float z)
{
    ScePspFVector3 scale = {x,y,z};
    sceGumScale(&scale);
}

void xGumScaleSingle(float scale)
{
    xGumScaleMulti(scale, scale, scale);
}

void xGumFrustumCullStart(float x_low, float y_low, float z_low, float x_high, float y_high, float z_high)
{
    VertexF* bounding_box = (VertexF*)sceGuGetMemory(8*sizeof(VertexF));
    bounding_box[0].x = x_high; bounding_box[0].y = y_high; bounding_box[0].z = z_high;
    bounding_box[0].x = x_high; bounding_box[0].y = y_high; bounding_box[0].z = z_low;
    bounding_box[0].x = x_high; bounding_box[0].y = y_low;  bounding_box[0].z = z_high;
    bounding_box[0].x = x_high; bounding_box[0].y = y_low;  bounding_box[0].z = z_low;
    bounding_box[0].x = x_low;  bounding_box[0].y = y_high; bounding_box[0].z = z_high;
    bounding_box[0].x = x_low;  bounding_box[0].y = y_high; bounding_box[0].z = z_low;
    bounding_box[0].x = x_low;  bounding_box[0].y = y_low;  bounding_box[0].z = z_high;
    bounding_box[0].x = x_low;  bounding_box[0].y = y_low;  bounding_box[0].z = z_low;
    
    sceGumUpdateMatrix();
    sceGuBeginObject(GU_VERTEX_32BITF, 8, 0, bounding_box);
}

void xGumFrustumCullEnd()
{
    sceGuEndObject();
}

GenVert __attribute__((aligned(16))) xGenericVert[4] =
{
    {0,     0, -0.5f, -0.5f,  0.0f},
    {0,   127, -0.5f,  0.5f,  0.0f},
    {127, 127,  0.5f,  0.5f,  0.0f},
    {127,   0,  0.5f, -0.5f,  0.0f}
};
