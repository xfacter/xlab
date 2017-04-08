#include <malloc.h>
#include <pspgu.h>
#include <pspgum.h>
#include <psptypes.h>
#include "xgraphics.h"
#include "xtexture.h"
#include "xmath.h"

#include "xparticle.h"

xParticle::xParticle()
{
    life = 0;
}

bool xParticle::Alive()
{
    if (life <= 0) return 0;
    return 1;
}

void xParticle::Init(ScePspFVector3* position, ScePspFVector3* velocity, float _fade)
{
    life = LIFE_PRECISION;
    pos.x = position->x;
    pos.y = position->y;
    pos.z = position->z;
    vel.x = velocity->x;
    vel.y = velocity->y;
    vel.z = velocity->z;
    fade = _fade;
}

void xParticle::Kill()
{
    life = 0;
}

u8 xParticle::Alpha()
{
    if (!Alive()) return 0;
    return (u8)(255.0f*(life/LIFE_PRECISION));
}

unsigned int xParticle::ColorAlpha(unsigned int color)
{
    color = color & 0xffffff;
    color = color | Alpha() << 24;
    return color;
}

void xParticle::Update(float delta_time, ScePspFVector3* accel)
{
    pos.x += vel.x*delta_time;
    pos.y += vel.y*delta_time;
    pos.z += vel.z*delta_time;
    
    vel.x += accel->x*delta_time;
    vel.y += accel->y*delta_time;
    vel.z += accel->z*delta_time;
    
    life -= fade*delta_time;
}

void xParticle::Render(ScePspFMatrix4* view_mat, float size, unsigned int color, u8 fix_alpha)
{
    if (size == 0) return;
    if (size < 0) size = -size*((LIFE_PRECISION - life)/LIFE_PRECISION);
    
    color = ColorAlpha(color);
    
    ScePspFVector3 translate;
    x_billboard(&translate, pos, view_mat);
    
    sceGumLoadIdentity();
    sceGumTranslate(&translate);
    xGumScaleMulti(size, size, 0);
    sceGuColor(color);
    sceGumDrawArray(xGenericVert_prim, xGenericVert_vtype|GU_TRANSFORM_3D, xGenericVert_count, 0, xGenericVert);
}

xParticleSystem::xParticleSystem(int num_particles)
{
    if (num_particles < 0) num_particles *= -1;
    num_part = num_particles;
    particle = new xParticle[num_part];
    
    source.x = 0; source.y = 0; source.z = 0;
    velocity.x = 0; velocity.y = 0; velocity.z = 0;
    vel_range.x = 0; vel_range.y = 0; vel_range.z = 0;
    accel.x = 0; accel.y = 0; accel.z = 0;
}

xParticleSystem::~xParticleSystem()
{
    if (particle) delete[] particle;
    particle = 0;
}

void xParticleSystem::SetAll(ScePspFVector3* _source, ScePspFVector3* source_rand_range, ScePspFVector3* _velocity, ScePspFVector3* vel_rand_range, ScePspFVector3* _accel, float min_sec, float max_sec)
{
    source.x = _source->x; source.y = _source->y; source.z = _source->z;
    source_range.x = source_rand_range->x; source_range.y = source_rand_range->y; source_range.z = source_rand_range->z;
    velocity.x = _velocity->x; velocity.y = _velocity->y; velocity.z = _velocity->z;
    vel_range.x = vel_rand_range->x; vel_range.y = vel_rand_range->y; vel_range.z = vel_rand_range->z;
    accel.x = _accel->x; accel.y = _accel->y; accel.z = _accel->z;
    SetFadeSpeed(min_sec, max_sec);
}

void xParticleSystem::SetFadeSpeed(float min_sec, float max_sec)
{
    fade_min = LIFE_PRECISION/max_sec;
    fade_max = LIFE_PRECISION/min_sec;
}

void xParticleSystem::InitParticle(int num)
{
    ScePspFVector3 vel = {velocity.x + x_randf(-vel_range.x, vel_range.x),
                          velocity.y + x_randf(-vel_range.y, vel_range.y),
                          velocity.z + x_randf(-vel_range.z, vel_range.z)};
    ScePspFVector3 src = {source.x + x_randf(-source_range.x, source_range.x),
                          source.y + x_randf(-source_range.y, source_range.y),
                          source.z + x_randf(-source_range.z, source_range.z)};
    particle[num].Init(&src, &vel, x_randf(fade_min, fade_max));
}

void xParticleSystem::KillSystem()
{
    for (int i = 0; i < num_part; i++)
    {
        particle[i].Kill();
    }
}

void xParticleSystem::RenderSystem(xTexture* part_tex, float size, unsigned int color, u8 fix_alpha)
{
    part_tex->SetImage(0, 0);
    sceGuTexFunc(GU_TFX_MODULATE,GU_TCC_RGBA);
    xGuTexFilter(0);
    
    xGuBlendDefault();
    
    sceGuDepthMask(1);
    
    xGuSaveStates();
    sceGuEnable(GU_TEXTURE_2D);
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_LIGHTING);
    
    ScePspFMatrix4 view_mat;
    sceGumMatrixMode(GU_VIEW);
    sceGumStoreMatrix(&view_mat);
    sceGumLoadIdentity();
    sceGumMatrixMode(GU_MODEL);
    
    for (int i = 0; i < num_part; i++)
    {
        if (particle[i].Alive()) particle[i].Render(&view_mat, size, color, fix_alpha);
    }
    
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadMatrix(&view_mat);
    sceGumMatrixMode(GU_MODEL);
    
    xGuLoadStates();
    
    sceGuDepthMask(0);
    
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
}

xEmitter::xEmitter(int num_particles) : xParticleSystem(num_particles)
{
    //Reserved
}

xEmitter::xEmitter(int num_particles, float delta_time_between_part, u8 particles_per_emit) : xParticleSystem(num_particles)
{
    Set(delta_time_between_part, particles_per_emit);
}

void xEmitter::Set(float delta_time_between_part, u8 particles_per_emit)
{
    dt_per_part = delta_time_between_part;
    part_per_emit = particles_per_emit;
}

void xEmitter::UpdateSystem(float delta_time)
{
    time_passed += delta_time;
    u8 emitted = 0;
    for (int i = 0; i < num_part; i++)
    {
        if (particle[i].Alive()) particle[i].Update(delta_time, &accel);
        else if (time_passed > dt_per_part)
        {
            InitParticle(i);
            emitted++;
            if (emitted >= part_per_emit) time_passed -= dt_per_part;
        }
    }
}

xExplosion::xExplosion(int num_particles) : xParticleSystem(num_particles)
{
    //Reserved
}

void xExplosion::UpdateSystem(float delta_time)
{
    for (int i = 0; i < num_part; i++)
    {
        if (particle[i].Alive()) particle[i].Update(delta_time, &accel);
    }
}

bool xExplosion::Exploding()
{
    for (int i = 0; i < num_part; i++)
    {
        if (particle[i].Alive()) return 1;
    }
    return 0;
}

void xExplosion::Explode(float x, float y, float z, int num_particles)
{
    source.x = x; source.y = y; source.z = z;
    
    if (num_particles < 0) num_particles = num_part + num_particles;
    if (num_particles > num_part || num_particles <= 0) num_particles = num_part;
    
    for (int i = 0; i < num_part; i++)
    {
        if (!particle[i].Alive())
        {
            InitParticle(i);
            num_particles--;
            if (num_particles <= 0) break;
        }
    }
}
