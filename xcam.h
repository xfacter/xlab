/**
 * This file belongs to the 'xlab' game engine.
 * Copyright 2009 xfacter
 * Copyright 2016 wickles
 * This work is licensed under the LGPLv3
 * subject to all terms as reproduced in the included LICENSE file.
 */

#pragma once

#include "xconfig.h"

/* NOTE
View Matrix!
------------------------------------------
                   Position Along Vector
Right x.x y.x z.x | w.x
Up    x.y y.y z.y | w.y
Look  x.z y.z z.z | w.z
       0   0   0  |  1
------------------------------------------
To get the camera position
-x = w.x*x.x + w.y*x.y + w.z*x.z = DotProduct(w, x);
-y = w.x*y.x + w.y*y.y + w.z*y.z = DotProduct(w, y);
-z = w.x*z.x + w.y*z.y + w.z*z.z = DotProduct(w, z);
*/

#if 1 // CXX

class xCamera
{
private:
    ScePspFMatrix4 matrix;
    ScePspFVector3 rot;
public:
    ScePspFVector3 pos;

    xCamera();
    xCamera(float x, float y, float z);
    void Reset(float x, float y, float z);
    void LookAt(ScePspFVector3* at, float radius);
    void MoveTo(ScePspFVector3* _pos);
    void Translate(ScePspFVector3* trans);
    void Navigate(float right, float up, float fwd);
    void RotateX(float rad);
    void RotateY(float rad);
    void RotateZ(float rad);
    void UpdateMatrix();
    void SetViewMatrix(int no_update);
    ScePspFVector3* GetPos();
};

/* not finished. need to implement angle function */
class xFollowCam
{
private:
    ScePspFVector3 pos;
    float threshold;
public:
    xFollowCam(ScePspFVector3* start, float max_dist);
    void Update(ScePspFVector3* follow, float dt);
    void SetCamera(float height, ScePspFVector3* follow, ScePspFVector3* up);
};

#else // C

typedef struct {
    ScePspFVector3 up;
    ScePspFVector3 pos;
    ScePspFVector3 dir;
} xCamera;

xCamSetMatrix(xCamera* cam);

xCamSetUpVector(xCamera* cam, float x, float y, float z);

//position, direction
xCamOrient(xCamera* cam, float px, float py, float pz, float dx, float dy, float dz);

//target, direction, radius
xCamOrbit(xCamera* cam, float tx, float ty, float tz, float dx, float dy, float dz, float r);

xCamMove(xCamera* cam, float fwd, float left, float up);

xCamRotateX(xCamera* cam, float angle);

xCamRotateY(xCamera* cam, float angle);

xCamRotateZ(xCamera* cam, float angle);

xCamRotateAroundX(xCamera* cam, float angle, float tx, float ty, float tz);

xCamRotateAroundY(xCamera* cam, float angle, float tx, float ty, float tz);

xCamRotateAroundZ(xCamera* cam, float angle, float tx, float ty, float tz);

#endif
