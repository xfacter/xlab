/**
 * This file belongs to the 'xlab' game engine.
 * Copyright 2009 xfacter
 * Copyright 2016 wickles
 * This work is licensed under the LGPLv3
 * subject to all terms as reproduced in the included LICENSE file.
 */

#ifndef __X_MD2_H__
#define __X_MD2_H__

#include "xconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int num_frames;
    int num_verts;
    int num_glcmds;
    ScePspFVector3* vertices;
    u8* normal_indices;
    int* gl_commands;
} xMd2;

typedef struct
{
    int anim;
    int frame_cur;
    int frame_next;
    int def_anim;
    int num_anims;
    float time;
    float interp;
    x_anim* anims_use;
    xMd2* md2_use;
} xMd2Object;

enum md2Anims
{
    MD2_STAND = 0,
    MD2_RUN,
    MD2_ATTACK,
    MD2_PAIN_A,
    MD2_PAIN_B,
    MD2_PAIN_C,
    MD2_JUMP,
    MD2_FLIP,
    MD2_SALUTE,
    MD2_FALLBACK,
    MD2_WAVE,
    MD2_POINT,
    MD2_CROUCH_STAND,
    MD2_CROUCH_WALK,
    MD2_CROUCH_ATTACK,
    MD2_CROUCH_PAIN,
    MD2_CROUCH_DEATH,
    MD2_DEATH_FALLBACK,
    MD2_DEATH_FALLFORWARD,
    MD2_DEATH_FALLBACKSLOW,
    MD2_BOOM
};

int xMd2Load(xMd2* my_md2, char* filename);

void xMd2Free(xMd2* my_md2);

void xMd2AnimBind(xMd2Object* object, xMd2* my_md2, x_anim* my_anims, int num_anim, int default_anim);

void xMd2AnimSet(xMd2Object* object, int anim, int loop, int ifnot);

void xMd2AnimUpdate(xMd2Object* object, float dt);

void xMd2AnimDraw(xMd2Object* object);

#ifdef __cplusplus
}
#endif

#endif
