#ifndef __X_MATH_H__
#define __X_MATH_H__

#include "xconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define X_PI     (3.1415926536f)
#define X_2PI    (6.2831853072f)
#define X_PI_2   (1.5707963268f)
#define X_PI_3   (1.0471975512f)
#define X_PI_4   (0.7853981634f)
#define X_PI_6   (0.5235987756f)
#define X_EULER  (2.7182818285f)
#define X_ROOT2  (1.4142135624f)
#define X_ROOT3  (1.7320508076f)
#define X_PI_180 (0.0174532925f)
#define X_180_PI (57.2957795130f)

#define DEG_TO_RAD(DEG) ((DEG)*X_PI_180)
#define RAD_TO_DEG(RAD) ((RAD)*X_180_PI)
#define SQR(X) ((X)*(X))

/* possibly add an error variable to see if a domain or other error occurred */

void  x_srand(u32 s);
void  x_auto_srand();
int   x_randi(int min, int max);
float x_randf(float min, float max);
float x_recip(float x);
float x_sqrtf(float x);
float x_modf(float x, float y);
float x_sinf(float x);
float x_cosf(float x);
float x_tanf(float x);
float x_asinf(float x);
float x_acosf(float x);
float x_atanf(float x);
float x_atan2f(float y, float x);
float x_sinhf(float x);
float x_coshf(float x);
float x_tanhf(float x);
void  x_sincos(float rad, float* sin, float* cos);
float x_expf(float x);
float x_log10f(float x);
float x_logbf(float b, float x);
float x_logef(float x);
float x_powf(float x, float pow);
int   x_next_pow2(int x);
int   x_num_align(int x, int num);
int   x_absi(int x);
float x_absf(float x);
int   x_mini(int x, int y);
float x_minf(float x, float y);
int   x_maxi(int x, int y);
float x_maxf(float x, float y);
float x_floorf(float x);
float x_ceilf(float x);
float x_ipart(float x);
float x_fpart(float x);
float x_roundf(float x);
float x_angle_to_target(float eye_x, float eye_y, float target_x, float target_y);

void  x_lerp(ScePspFVector3* r, ScePspFVector3* v0, ScePspFVector3* v1, float t);
float x_magnitude(ScePspFVector3* v);
void  x_normalize(ScePspFVector3* v, float mag);
float x_dotproduct(ScePspFVector3* v0, ScePspFVector3* v1);
void  x_crossproduct(ScePspFVector3* r, ScePspFVector3* v0, ScePspFVector3* v1);
void  x_normal(ScePspFVector3* r, ScePspFVector3* p1, ScePspFVector3* p2, ScePspFVector3* p3);
void  x_translate(ScePspFVector3* v, ScePspFVector3* trans);
void  x_rotatex(ScePspFVector3* v, float x);
void  x_rotatey(ScePspFVector3* v, float x);
void  x_rotatez(ScePspFVector3* v, float x);
void  x_billboard(ScePspFVector3* r, ScePspFVector3* pos, ScePspFMatrix4* view_mat);
void  x_billboard_dir(ScePspFVector3* r, ScePspFVector3* eye, ScePspFVector3* pos, ScePspFVector3* dir);

void  x_ease_to_target1(float* cur, float target, float p, float dt);
void  x_ease_to_target2(float* cur_x, float* cur_y, float target_x, float target_y, float p, float dt);
void  x_ease_to_target3(ScePspFVector3* cur, ScePspFVector3* target, float p, float dt);

float x_dist2(float x1, float y1, float x2, float y2);
float x_dist3(ScePspFVector3* p1, ScePspFVector3* p2);
int   x_dist_test2(float x1, float y1, float x2, float y2, float d);
int   x_dist_test3(ScePspFVector3* p1, ScePspFVector3* p2, float d);

#ifdef __cplusplus
}
#endif

#endif
