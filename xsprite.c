/**
 * This file belongs to the 'xlab' game engine.
 * Copyright 2009 xfacter
 * Copyright 2016 wickles
 * This work is licensed under the LGPLv3
 * subject to all terms as reproduced in the included LICENSE file.
 */

#include "xsprite.h"

int xSpriteBind(xSprite* object, xTexture* texture, x_anim* my_anims, int num_anim, int fwidth, int fheight, int default_anim)
{
    if (!object || !texture || !my_anims) return 0;
    object->texture = texture;
    object->uframes = texture->width/fwidth;
    object->vframes = texture->height/fheight;
    object->frame_width = fwidth;
    object->frame_height = fheight;
    object->anims_use = my_anims;
    object->num_anims = num_anim;
    xSpriteSetAnim(object, default_anim, 1);
    return 1;
}

void xSpriteSetAnim(xSprite* object, int anim, int loop)
{
    if (!object || !object->anims_use) return;
    if (anim < 0 || anim >= object->num_anims) anim = 0;
    object->anim = anim;
    if (loop) object->def_anim = anim;
    object->frame = object->anims_use[object->anim].start;
    object->time = 0.0f;
}

void xSpriteUpdate(xSprite* object, float dt)
{
    if (!object || !object->anims_use) return;
    object->time += dt;
    while (object->time > object->anims_use[object->anim].delay)
    {
        object->time -= object->anims_use[object->anim].delay;
        object->frame += 1;
        if (object->frame >= object->anims_use[object->anim].start + object->anims_use[object->anim].size)
        {
            xSpriteSetAnim(object, object->def_anim, 0);
        }
    }
}

void xSpriteDraw(xSprite* object, int x, int y)
{
    if (!object || !object->texture) return;
    //xTexDrawSect(object->texture, x, y, object->frame*object->frame_width, object->anim*object->frame_height, object->frame_width, object->frame_height);
    xTexDraw(object->texture, x, y,
             object->frame_width, object->frame_height,
             object->frame_width * (object->frame % object->uframes), object->frame_height * (object->frame / object->uframes),
             object->frame_width, object->frame_height);
}

#if 0
void xSpriteDraw3D(xSprite* object, ScePspFVector3* pos, float length, float height)
{
    if (!object || !object->texture) return;
    xTexDrawSprite(object->texture, pos, length, height,
                   object->frame_width * (object->frame % object->uframes), object->frame_height * (object->frame / object->uframes),
                   object->frame_width, object->frame_height);
}
#endif
