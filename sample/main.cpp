/**
 * This file belongs to the 'xlab' game engine.
 * Copyright 2009 xfacter
 * Copyright 2016 wickles
 * This work is licensed under the LGPLv3
 * subject to all terms as reproduced in the included LICENSE file.
 */

#include <stdio.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspkernel.h>
#include "xlab.h"
#include "xmem.h"
#include "xctrl.h"
#include "xtime.h"
#include "xgraphics.h"
#include "xsound.h"
#include "xtexture.h"
#include "xmd2.h"
#include "xmath.h"
#include "xtext.h"
#include "xobj.h"
#include "xprim.h"
#include "xcam.h"
//#include "xdraw.h"
//#include "xsprite.h"
//#include "xscreen.h"
//#include "xlist.h"

#define DEADZONE (0.20f)

int xMain()
{
	xTimeInit();
	xCtrlInit();

	xGuInit();
	//xGuEnable(X_DITHER_SMOOTH);
	//xGuEnable(X_PSEUDO_AA);
	xGuDisable(X_WAIT_VBLANK);
	xGuTexMode(GU_TFX_MODULATE, 1);
	sceGuEnable(GU_TEXTURE_2D);
	xGuTexFilter(X_BILINEAR);
	xGuPerspective(75.0f, 0.5f, 1000.0f);
	sceGuColor(0xffffffff);

	sceGuEnable(GU_LIGHTING);
	sceGuEnable(GU_LIGHT0);
	ScePspFVector3 lightPosition = {0.0f, 0.0f, 100.0f};
	sceGuLight(0, GU_POINTLIGHT, GU_DIFFUSE|GU_SPECULAR, &lightPosition );
	sceGuLightColor(0, GU_DIFFUSE, 0xff7f7f7f);
	sceGuLightColor(0, GU_SPECULAR, 0xffffffff);
	sceGuSpecular(12.0f);
	sceGuAmbient(0x00222222);

	xSoundInit(16);

	xTexture* font_tex = xTexLoadTGA("./font.tga", 0, X_TEX_TOP_IN_VRAM|X_TEX_GRAY_TO_ALPHA);
	xBitmapFont* font = xTextLoadFont(font_tex, "./width.fw");
	xTextSetFont(font);
	xTextSetColor(0xffffffff);
	xTextSetScale(1.0f);
	xTextSetAlign(X_ALIGN_LEFT);

	xMd2 monster_mdl;
	xMd2Load(&monster_mdl, "./monster.md2");
	xMd2 weapon_mdl;
	xMd2Load(&weapon_mdl, "./weapon.md2");
	xMd2Object monster_anim;
	xMd2Object weapon_anim;
	xMd2AnimBind(&monster_anim, &monster_mdl, 0, 0, 0);
	xMd2AnimBind(&weapon_anim, &weapon_mdl, 0, 0, 0);
	xTexture* monster_tex = xTexLoadTGA("./monster.tga", 0, X_TEX_TOP_IN_VRAM);
	xTexture* weapon_tex = xTexLoadTGA("./weapon.tga", 0, X_TEX_TOP_IN_VRAM);
	xSoundBuffer* my_sound = xSoundLoadBufferWav("./sound.wav");

	xCamera cam(-30.0f, 60.0f, -30.0f);
	cam.RotateX(-X_PI/2.0f);
	//cam.RotateZ(X_PI/2.0f);
	int cur_anim = 0;
	int pl_control = 0;
	ScePspFVector3 character_pos = {0., 0., 0.};
	float character_rot = 0.;

	while(xRunning())
	{
		xTimeUpdate();
		float x_dt = xTimeGetDeltaTime();
		xCtrlUpdate(x_dt);
		ScePspFVector3 trans = {0.0f, 0.0f, 0.0f};
		if (pl_control)
		{
			if (xCtrlAnalogAlive(DEADZONE))
			{
				const float move_rate = 50.0f;
				character_pos.x += move_rate*xCtrlAnalogX()*x_dt;
				character_pos.y += move_rate*xCtrlAnalogY()*x_dt;
				character_rot = x_atan2f(xCtrlAnalogY(), xCtrlAnalogX());
				if (monster_anim.anim != MD2_RUN)
				{
					xMd2AnimSet(&monster_anim, MD2_RUN, 1, 1);
					xMd2AnimSet(&weapon_anim, MD2_RUN, 1, 1);
				}
			}
			else
			{
				if (monster_anim.anim != MD2_STAND)
				{
					xMd2AnimSet(&monster_anim, MD2_STAND, 1, 1);
					xMd2AnimSet(&weapon_anim, MD2_STAND, 1, 1);
				}
			}
		}
		else
		{
			if (xCtrlAnalogAlive(DEADZONE))
			{
				const float rot_rate = 1.0f;
				cam.RotateY(rot_rate*xCtrlAnalogX()*x_dt);
				cam.RotateX(rot_rate*xCtrlAnalogY()*x_dt);
			}
			if (xCtrlPress(X_CTRL_LTRIGGER)) cam.RotateZ(-X_PI/2.0f*x_dt);
			if (xCtrlPress(X_CTRL_RTRIGGER)) cam.RotateZ(X_PI/2.0f*x_dt);
			const float cam_rate = 30.0f;
			if (xCtrlPress(X_CTRL_SQUARE)) trans.x += cam_rate*x_dt;
			if (xCtrlPress(X_CTRL_CIRCLE)) trans.x -= cam_rate*x_dt;
			if (xCtrlPress(X_CTRL_UP)) trans.y += cam_rate*x_dt;
			if (xCtrlPress(X_CTRL_DOWN)) trans.y -= cam_rate*x_dt;
			if (xCtrlPress(X_CTRL_TRIANGLE)) trans.z += cam_rate*x_dt;
			if (xCtrlPress(X_CTRL_CROSS)) trans.z -= cam_rate*x_dt;
			if (xCtrlTap(X_CTRL_LEFT) && cur_anim > 0)
			{
				cur_anim -= 1;
				xMd2AnimSet(&monster_anim, cur_anim, 1, 1);
				xMd2AnimSet(&weapon_anim, cur_anim, 1, 1);
			}
			if (xCtrlTap(X_CTRL_RIGHT) && cur_anim < monster_anim.num_anims-1)
			{
				cur_anim += 1;
				xMd2AnimSet(&monster_anim, cur_anim, 1, 1);
				xMd2AnimSet(&weapon_anim, cur_anim, 1, 1);
			}
			if (xCtrlTap(X_CTRL_START)) xSoundPlay(my_sound);
		}

		if (xCtrlTap(X_CTRL_SELECT)) pl_control = !pl_control;

		xMd2AnimUpdate(&monster_anim, x_dt);
		xMd2AnimUpdate(&weapon_anim, x_dt);

		cam.Navigate(trans.x, trans.y, trans.z);

		xGuClear(0xffaf3f00);
		cam.SetViewMatrix(0);

		sceGumLoadIdentity();
		sceGuEnable(GU_TEXTURE_2D);
		sceGuEnable(GU_LIGHTING);
		xGuTexFilter(X_BILINEAR);
		xGuTexMode(GU_TFX_MODULATE, 0);
		sceGuColor(0xffffffff);
		xPrimPlane(300., 300., X_PRIM_NO_CULL);
		xGumTranslate(0.0f, 0.0f, 24.0f);
		xGumTranslate(character_pos.x, character_pos.y, character_pos.z);
		xGumRotateZ(character_rot);
		xTexSetImage(monster_tex);
		xMd2AnimDraw(&monster_anim);
		xTexSetImage(weapon_tex);
		xMd2AnimDraw(&weapon_anim);
		sceGuDisable(GU_TEXTURE_2D);

		xGuTexMode(GU_TFX_MODULATE, 1);
		xGuTexFilter(X_BILINEAR);
		//xTextPrintf(0, 0, "FPS: %f", 1.0f/x_dt);
		xTextPrintf(0, 0, "FPS: %d", xTimeFpsApprox());
		xGuFrameEnd();
	}

	xMd2Free(&monster_mdl);
	xTexFree(monster_tex);
	xTexFree(weapon_tex);
	xTexFree(font_tex);
	xTextFreeFont(font);

	xGuEnd();
	xSoundEnd();

	return 0;
}
