/**
 * This file belongs to the 'xlab' game engine.
 * Copyright 2009 xfacter
 * Copyright 2016 wickles
 * This work is licensed under the LGPLv3
 * subject to all terms as reproduced in the included LICENSE file.
 */

#pragma once

#include "xconfig.h"
#include "xtexture.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int anim;
    int frame;
    int def_anim;
    int num_anims;
    float time;
    int frame_width;
    int frame_height;
    int uframes;
    int vframes;
    x_anim* anims_use;
    xTexture* texture;
} xSprite;

/* uses anims going down the image, and frames from left to right */
int xSpriteBind(xSprite* object, xTexture* texture, x_anim* my_anims, int num_anim, int fwidth, int fheight, int default_anim);

void xSpriteSetAnim(xSprite* object, int anim, int loop);

void xSpriteUpdate(xSprite* object, float dt);

void xSpriteDraw(xSprite* object, int x, int y);

//void xSpriteDraw3D(xSprite* object, ScePspFVector3* pos, float length, float height);

#ifdef __cplusplus
}
#endif
