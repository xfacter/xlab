#include <pspgu.h>
#include <pspgum.h>
#include "xgraphics.h"
#include "xmath.h"
#include "xmem.h"

#include "xparticle.h"

xParticleSystem* xParticleSystemConstruct(u32 max)
{
    xParticleSystem* s = (xParticleSystem*)xMalloc(sizeof(xParticleSystem));
    if (!s) return 0;
    s->particles = (xParticle*)xMalloc(num*sizeof(xParticle));
    s->num = max;
    s->counter = 0;
    s->time = 0.0f;
    if (!s->particles)
    {
        xFree(s);
        return 0;
    }
    return s;
}

void xParticleSystemDestroy(xParticleSystem* s)
{
    if (!s) return;
    if (!s->particles) xFree(s->particles);
    xFree(s);
}

static void x_particle_init_next(xParticleSystem* s)
{
    int n = s->counter;
    s->particles[n].pos.x = s->pos.x + x_randf(-s->pos_rand.x, s->pos_rand.x);
    s->particles[n].pos.y = s->pos.y + x_randf(-s->pos_rand.y, s->pos_rand.y);
    s->particles[n].pos.z = s->pos.z + x_randf(-s->pos_rand.z, s->pos_rand.z);
    s->particles[n].vel.x = s->vel.x + x_randf(-s->vel_rand.x, s->vel_rand.x);
    s->particles[n].vel.y = s->vel.y + x_randf(-s->vel_rand.y, s->vel_rand.y);
    s->particles[n].vel.z = s->vel.z + x_randf(-s->vel_rand.z, s->vel_rand.z);
    s->particles[n].size = s->size + x_randf(-s->size_rand, s->size_rand);
    s->particles[n].life = s->life + x_randf(-s->life_rand, s->life_rand);
    s->counter += 1;
    if (s->counter >= s->num) s->counter = 0;
}

void xParticleSystemBurst(xParticleSystem* s, u32 num)
{
    if (!s) return;
    while (num > 0)
    {
        x_particle_init_next(s);
        num -= 1;
    }
}

void xParticleSystemUpdate(xParticleSystem* s, float dt)
{
    if (!s) return;
    int i;
    for (i = 0; i < s->num; i++)
    {
        if (s->particles[i].life > 0.0f)
        {
            s->particles[i].life -= dt;
            s->particles[i].size += dt*s->growth;
            s->particles[i].vel.x += s->accel.x;
            s->particles[i].vel.y += s->accel.y;
            s->particles[i].vel.z += s->accel.z;
            s->particles[i].pos.x += s->vel.x;
            s->particles[i].pos.y += s->vel.y;
            s->particles[i].pos.z += s->vel.z;
        }
    }
    s->time += dt;
    time_per = x_recip(s->rate);
    while (s->time >= time_per)
    {
        x_particle_init_next(s);
        s->time -= time_per;
    }
}

typedef struct {
    float x, y, z;
} part_vert;

static part_vert __attribute__((aligned(16))) part_geom[2] = {
    { 0.0f,  0.0f,  0.0f},
    {-1.0f, -1.0f, -1.0f}
};

void xParticleSystemRender(xParticleSystem* s, int prim, int billboard)
{
    if (!s) return;
    sceGuDepthMask(GU_TRUE);
    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_LIGHTING);
    
    ScePspFMatrix4 view_mat;
    sceGumMatrixMode(GU_VIEW);
    sceGumStoreMatrix(&view_mat);
    sceGumLoadIdentity();
    sceGumMatrixMode(GU_MODEL);
    
    ScePspFVector3 translate;
    int i;
    for (i = 0; i < s->num; i++)
    {
        if (s->particles[i].life > 0.0f)
        {
            x_billboard(&translate, &s->particles[i].pos, view_mat);
            sceGumLoadIdentity();
            sceGumTranslate(&translate);
            switch (prim)
            {
                case X_PARTICLE_LINE:
                    sceGumScale(&s->particles[i].vel);
                    sceGumDrawArray(GU_LINES, GU_VERTEX_32BITF|GU_TRANFORM_3D, 2, 0, part_geom);
                    break;
                case X_PARTICLE_POINT:
                    sceGumDrawArray(GU_POINTS, GU_VERTEX_32BITF|GU_TRANFORM_3D, 1, 0, part_geom);
                    break;
                default:
                    xGumScale(size, size, 1.0f);
                    //color/fade?
                    xGumDrawUnitTexQuad();
                    break;
            }
        }
    }
    
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadMatrix(&view_mat);
    sceGumMatrixMode(GU_MODEL);
    
    xGuLoadStates();
    sceGuDepthMask(GU_FALSE);
}
