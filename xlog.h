/**
 * This file belongs to the 'xlab' game engine.
 * Copyright 2009 xfacter
 * Copyright 2016 wickles
 * This work is licensed under the LGPLv3
 * subject to all terms as reproduced in the included LICENSE file.
 */

#ifndef __X_LOG_H__
#define __X_LOG_H__

#include "xconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

void xLogPrintf(char* text, ... );

#define X_LOG xLogPrintf

#ifdef __cplusplus
}
#endif

#endif
