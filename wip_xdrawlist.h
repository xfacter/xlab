/**
 * This file belongs to the 'xlab' game engine.
 * Copyright 2009 xfacter
 * Copyright 2016 wickles
 * This work is licensed under the LGPLv3
 * subject to all terms as reproduced in the included LICENSE file.
 */

#pragma once

#include "xconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/* this will maybe not work for sceGuDrawArray if data is also placed in list because it will be at a different place */
/* ignore above, should work with realloc now */

void xListStart(u32 test);

void* xListFinish();

void xListDestroy(void* ptr);

#ifdef __cplusplus
}
#endif
