#ifndef __X_PARTICLE_H__
#define __X_PARTICLE_H__

#include "xconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define X_PARTICLE_QUAD (0)
#define X_PARTICLE_LINE (1)
#define X_PARTICLE_POINT (2)

typedef struct {
    ScePspFVector3 pos;
    ScePspFVector3 vel;
    float size;
    //moar?
    float life;
    //how to handle fade values?
} xParticle;

typedef struct {
    ScePspFVector3 pos;
    ScePspFVector3 pos_rand;
    ScePspFVector3 vel;
    ScePspFVector3 vel_rand;
    ScePspFVector3 accel;
    float size;
    float size_rand;
    float life;
    float life_rand;
    u32 rate;
    float growth;
    //moar?
    float time;
    u32 counter;
    u32 num;
    xParticle* particles;
} xParticleSystem;

xParticleSystem* xParticleSystemConstruct(u32 max);
void xParticleSystemDestroy(xParticleSystem* s);
void xParticleSystemBurst(xParticleSystem* s, u32 num);
void xParticleSystemUpdate(xParticleSystem* s, float dt);
void xParticleSystemRender(xParticleSystem* s, int prim, int billboard);
//void xParticleSystemRender(xParticleSystem* s, ScePspFVector3* cam, int prim, int billboard);

#ifdef __cplusplus
}
#endif

#endif
