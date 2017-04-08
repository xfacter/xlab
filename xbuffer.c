#include <pspgu.h>
#include "xmem.h"
#include "xmath.h"
#include "xgraphics.h"

#include "xbuffer.h"

xBuffer* xBufferConstruct(int psm, int width, int height)
{
    xBuffer* b = (xBuffer*)x_malloc(sizeof(xBuffer));
    if (!b) return 0;

    int bpp = 0;
    switch (psm)
    {
        case GU_PSM_5650:
        case GU_PSM_5551:
        case GU_PSM_4444:
            bpp = 16;
            break;
        default:
            bpp = 32;
            psm = GU_PSM_8888;
            break;
    }
    b->psm = psm;
    b->width = width;
    b->height = height;
    b->buf_width = x_next_pow2(width);
    b->pow2_height = x_next_pow2(height);
    b->x_scale = (float)b->width/b->buf_width;
    b->y_scale = (float)b->height/b->pow2_height;
    b->data = x_valloc(bpp*b->buf_width*b->height/8);
    if (!b->data)
    {
        x_free(b);
        return 0;
    }
    return b;
}

void xBufferDestroy(xBuffer* b)
{
    if (!b) return;
    x_free(b->data);
}

void xBufferSetImage(xBuffer* b)
{
    if (!b)
    {
        xGuSetDebugTex();
    }
    else
    {
        xGuTexFunc(0, b->x_scale, b->y_scale, b->psm, b->buf_width, b->pow2_height, b->buf_width, b->data);
    }
}

static const float screen_x_scale = (float)X_SCREEN_WIDTH/X_BUFFER_WIDTH;
static const float screen_y_scale = (float)X_SCREEN_HEIGHT/512;
extern int x_buf_psm;
static xBuffer frame_buf;
xBuffer* xBufferFrameBuffer()
{
    frame_buf.psm = x_buf_psm;
    frame_buf.width = X_SCREEN_WIDTH;
    frame_buf.height = X_SCREEN_HEIGHT;
    frame_buf.buf_width = X_BUFFER_WIDTH;
    frame_buf.pow2_height = 512;
    frame_buf.x_scale = screen_x_scale;
    frame_buf.y_scale = screen_y_scale;
    frame_buf.data = xGuDrawPtr(0, 1);
    return &frame_buf;
}

void xBufferRenderToScreen(xBuffer* b)
{
    /* change to slice draw - maybe put tex draw func in xgraphics.h to be used in xtexture.h and elsewhere */
    if (!b) return;
    xBufferSetImage(b);
    TVertex2D* vertices = (TVertex2D*)sceGuGetMemory(2*sizeof(TVertex2D));
    vertices[0].u = 0;
    vertices[0].v = b->height;
    vertices[0].x = 0;
    vertices[0].y = 0;
    vertices[0].z = 0;
    vertices[1].u = b->width;
    vertices[1].v = 0;
    vertices[1].x = X_SCREEN_WIDTH;
    vertices[1].y = X_SCREEN_HEIGHT;
    vertices[1].z = 0;
    sceGuDrawArray(GU_SPRITES, TVertex2D_vtype|GU_TRANSFORM_2D, 2, 0, vertices);
}

void xBufferRenderFromScreen(xBuffer* b)
{
    //...
}

void xBufferSetRenderTarget(xBuffer* b)
{
    if (!b) return;
    xGuRenderTarget(b->psm, b->width, b->height, b->buf_width, b->data);
}

/* is this really necessary? */
void xBufferRenderReset()
{
    xGuRenderReset();
}
