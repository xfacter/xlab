/**
 * This file belongs to the 'xlab' game engine.
 * Copyright 2009 xfacter
 * Copyright 2016 wickles
 * This work is licensed under the LGPLv3
 * subject to all terms as reproduced in the included LICENSE file.
 */

#ifndef __X_LIB_H__
#define __X_LIB_H__

#include "xconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

int xRunning();
void xExit();

/* main function, defined by user, ran through real main */
extern int xMain();

#ifdef __cplusplus
}
#endif

#endif
