#ifndef __X_PARTICLE_H__
#define __X_PARTICLE_H__

#include <psptypes.h>
#include "xtexture.h"

#define LIFE_PRECISION (1000.0f)

class xParticle
{
private:
    ScePspFVector3 pos;
    ScePspFVector3 vel;
    float fade;
    float life;
public:
    xParticle();
    bool Alive();
    void Init(ScePspFVector3* position, ScePspFVector3* velocity, float _fade);
    void Kill();
    u8 Alpha();
    unsigned int ColorAlpha(unsigned int color);
    void Update(float delta_time, ScePspFVector3* accel);
    void Render(ScePspFMatrix4* view_mat, float size, unsigned int color, u8 fix_alpha);
};

class xParticleSystem
{
protected:
    xParticle* particle;
    int num_part;
    float fade_min;
    float fade_max;
public:
    xParticleSystem(int num_particles);
    virtual ~xParticleSystem();
    
    ScePspFVector3 source;
    ScePspFVector3 source_range;
    ScePspFVector3 velocity;
    ScePspFVector3 vel_range;
    ScePspFVector3 accel;
    
    void SetAll(ScePspFVector3* _source, ScePspFVector3* source_rand_range, ScePspFVector3* _velocity, ScePspFVector3* vel_rand_range, ScePspFVector3* _accel, float min_sec, float max_sec);
    void SetFadeSpeed(float min_sec, float max_sec);
    virtual void InitParticle(int num);
    virtual void UpdateSystem(float delta_time) = 0;
    virtual void KillSystem();
    virtual void RenderSystem(xTexture* part_tex, float size, unsigned int color, u8 fix_alpha);
};

class xEmitter : public xParticleSystem
{
private:
    float dt_per_part;
    u8 part_per_emit;
    float time_passed;
public:
    xEmitter(int num_particles);
    xEmitter(int num_particles, float delta_time_between_part, u8 particles_per_emit);
    
    void Set(float delta_time_between_part, u8 particles_per_emit);
    
    virtual void UpdateSystem(float delta_time);
};

class xExplosion : public xParticleSystem
{
private:
    //Reserved
public:
    xExplosion(int num_particles);
    virtual void UpdateSystem(float delta_time);
    bool Exploding();
    void Explode(float x, float y, float z, int num_particles);
};

#endif
