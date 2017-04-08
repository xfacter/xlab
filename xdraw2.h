#ifndef __X_ORTHO_X__
#define __X_ORTHO_X__

#include "xconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

void xOrthoStar(float radius, u16 sides, u16 add);

void xOrthoPolygon(float radius, u16 sides);

void xOrthoBox(float width, float height);

void xDraw3DRect(ScePspFVector3* pos, float length, float height, u32 color);

void xDraw3DLine(ScePspFVector3* cam, ScePspFVector3* p0, ScePspFVector3* p1, float h0, float h1, u32 c);

#ifdef __cplusplus
}
#endif

#endif
