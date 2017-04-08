#ifndef __X_BUFFER_H__
#define __X_BUFFER_H__

#include "xconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int psm;
    u16 width;
    u16 height;
    u16 buf_width;
    u16 pow2_height;
    float x_scale;
    float y_scale;
    void* data;
} xBuffer;

xBuffer* xBufferConstruct(int psm, int width, int height);

void xBufferDestroy(xBuffer* b);

void xBufferSetImage(xBuffer* b);

xBuffer* xBufferFrameBuffer();

void xBufferSetRenderTarget(xBuffer* b);

void xBufferRenderReset();

//void xBufferRenderFull

//void xBufferSomeKindOfEffect();

#ifdef __cplusplus
}
#endif

#endif
