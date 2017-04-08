#ifndef __X_LIST_H__
#define __X_LIST_H__

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

#endif
