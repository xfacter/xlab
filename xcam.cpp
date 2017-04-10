/**
 * This file belongs to the 'xlab' game engine.
 * Copyright 2009 xfacter
 * Copyright 2016 wickles
 * This work is licensed under the LGPLv3
 * subject to all terms as reproduced in the included LICENSE file.
 */

#include <pspgu.h>
#include <pspgum.h>
#include "xmath.h"

#include "xcam.h"

xCamera::xCamera()
{
    Reset(0.0f, 0.0f, 0.0f);
}

xCamera::xCamera(float x, float y, float z)
{
    Reset(x, y, z);
}

void xCamera::Reset(float x, float y, float z)
{
    gumLoadIdentity(&matrix);
    rot.x = 0.0f; rot.y = 0.0f; rot.z = 0.0f;
    pos.x = x; pos.y = y; pos.z = z;
}

void xCamera::LookAt(ScePspFVector3* at, float radius)
{
    //
}

void xCamera::MoveTo(ScePspFVector3* _pos)
{
    pos = *_pos;
}

void xCamera::Translate(ScePspFVector3* trans)
{
    pos.x += trans->x;
    pos.y += trans->y;
    pos.z += trans->z;
}

void xCamera::Navigate(float right, float up, float fwd)
{
    pos.x += right * matrix.x.x;
    pos.y += right * matrix.y.x;
    pos.z += right * matrix.z.x;

    pos.x += up * matrix.x.y;
    pos.y += up * matrix.y.y;
    pos.z += up * matrix.z.y;

    pos.x += fwd * matrix.x.z;
    pos.y += fwd * matrix.y.z;
    pos.z += fwd * matrix.z.z;

}

void xCamera::RotateX(float rad)
{
    rot.x += rad;
}
void xCamera::RotateY(float rad)
{
    rot.y += rad;
}
void xCamera::RotateZ(float rad)
{
    rot.z += rad;
}

void xCamera::UpdateMatrix()
{
    matrix.w.x = 0.0f; matrix.w.y = 0.0f; matrix.w.z = 0.0f;
    ScePspFMatrix4 rot_matrix;
    gumLoadIdentity(&rot_matrix);
    gumRotateXYZ(&rot_matrix, &rot);
    gumMultMatrix(&matrix, &rot_matrix, &matrix);
    gumTranslate(&matrix, &pos);
    rot.x = 0.0f; rot.y = 0.0f; rot.z = 0.0f;
}

void xCamera::SetViewMatrix(int no_update)
{
    sceGumMatrixMode(GU_VIEW);
    if (!no_update) UpdateMatrix();
    sceGumLoadMatrix(&matrix);
    sceGumMatrixMode(GU_MODEL);
}

ScePspFVector3* xCamera::GetPos()
{
    return &pos;
}

xFollowCam::xFollowCam(ScePspFVector3* start, float max_dist)
{
    pos = *start;
    threshold = max_dist;
}

void xFollowCam::Update(ScePspFVector3* follow, float dt)
{
    if (x_dist_test2(pos.x, pos.y, follow->x, follow->y, threshold))
    {
        ScePspFVector3 dir = {pos.x - follow->x, pos.y - follow->y, pos.z - follow->z};
        x_normalize(&dir, 1.0f);
        ScePspFVector3 move = {follow->x + threshold*dir.x, follow->y + threshold*dir.y, follow->z + threshold*dir.z};
        x_ease_to_target3(&pos, &move, 0.99f, dt);
    }
}

void xFollowCam::SetCamera(float height, ScePspFVector3* follow, ScePspFVector3* up)
{
    ScePspFVector3 eye = {pos.x + height*up->x, pos.y + height*up->y, pos.z + height*up->z};
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();
    sceGumLookAt(&eye, follow, up);
    sceGumMatrixMode(GU_MODEL);
}

#if 0
// better C version

typedef struct {
    ScePspFVector3 up;
    ScePspFVector3 pos;
    ScePspFVector3 dir;
} xCamera;

xCamSetMatrix(xCamera* cam);

xCamSetUpVector(xCamera* cam, float x, float y, float z);

xCamOrient(xCamera* cam, float px, float py, float pz, float dx, float dy, float dz);

xCamOrbit(xCamera* cam, float tx, float ty, float tz, float dx, float dy, float dz, float r);

xCamMove(xCamera* cam, float fwd, float left, float up);

xCamRotateX(xCamera* cam, float angle);

xCamRotateY(xCamera* cam, float angle);

xCamRotateZ(xCamera* cam, float angle);

xCamRotateAroundX(xCamera* cam, float angle, float tx, float ty, float tz);

xCamRotateAroundY(xCamera* cam, float angle, float tx, float ty, float tz);

xCamRotateAroundZ(xCamera* cam, float angle, float tx, float ty, float tz);

#endif
