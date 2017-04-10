/**
 * This file belongs to the 'xlab' game engine.
 * Copyright 2009 xfacter
 * Copyright 2016 wickles
 * This work is licensed under the LGPLv3
 * subject to all terms as reproduced in the included LICENSE file.
 */

#ifndef __X_CLIP_H__
#define __X_CLIP_H__

#include <psptypes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    //left, right, bottom, top, near, far
    ScePspFVector4 planes[6];
} xFrustum;

void xClipGetFrustum(xFrustum* out);

int xClipPointInFrustum(xFrustum* f, ScePspFVector3* p);

//int xClipDrawArrayFrustum(xFrustum* f, int prim, int vtype, int count, void* indices, void* vertices);

#ifdef __cplusplus
}
#endif

#endif
