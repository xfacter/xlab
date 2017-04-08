#ifndef __X_OBJ_H__
#define __X_OBJ_H__

#include "xconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int num_verts;
    int vtype;
    void* vertices;
} xObj;

int xObjLoad(xObj* object, char* filename);

void xObjFree(xObj* object);

/* experimental, doesnt seem to work */
//void xObjOptimize(xObj* object);

void xObjDraw(xObj* object);

#ifdef __cplusplus
}
#endif

#endif
