#ifndef __X_PRIM_H__
#define __X_PRIM_H__

#include "xconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define X_PRIM_TEXTURE (1<<0)
#define X_PRIM_NORMAL  (1<<1)
#define X_PRIM_NO_CULL (1<<2)

/* all prims use whatever color has been entered in sceGuColor last */

void xPrimTriangle(ScePspFVector3* v0, ScePspFVector3* v1, ScePspFVector3* v2, int attr);

void xPrimPlane(float width, float height, int attr);

void xPrimBox(float x_length, float y_length, float z_length, int attr);

void xPrimCube(float length, int attr);

void xPrimEllipsoid(float x_radius, float y_radius, float z_radius, int slices, int rows, int attr);

void xPrimSphere(float radius, int slices, int rows, int attr);

void xPrimPyramid(float length, float width, float height, int attr);

void xPrimCone(float radius, float height, int slices, int attr);

void xPrimCylinder(float radius, float height, int slices, int attr);

void xPrimTorus(float radius, float thickness, int slices, int rows, int attr);

#ifdef __cplusplus
}
#endif

#endif
