#ifndef __X_MATH_H__
#define __X_MATH_H__

#include <psptypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Math macros */

/*
  pi:    3.14159 26535 89793 23846 26433 83279 50288
  euler: 2.71828 18284 59045 23536 02874 71352 66249
  root2: 1.41421 35623 73095 04880 16887 24209 69807
  root3: 1.73205 08075 68877 29352 74463 41505 87236
*/
  
#define PI     (3.1415926536f)
#define TWO_PI (6.2831853072f)
#define PI_2   (1.5707963268f)
#define PI_3   (1.0471975512f)
#define PI_4   (0.7853981634f)
#define PI_6   (0.5235987756f)
#define EULER  (2.7182818285f)
#define ROOT2  (1.4142135624f)
#define ROOT3  (1.7320508076f)
#define SQR(X) ((X)*(X))

void  x_srand(unsigned int seed);
void  x_auto_srand();
int   x_randi(int min, int max);
float x_randf(float min, float max);
float x_itof(int x);
int   x_ftoi(float x);
float x_sqrtf(float x);
float x_inv_sqrtf(float x);
float x_inverse(float x);
int   x_even_odd(int x);
int   x_absi(int x);
float x_absf(float x);
float x_sinf(float rad);
float x_cosf(float rad);
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
float x_lnf(float x);
float x_powf(float x, float pow);
int   x_next_pow2i(int x);
int   x_modi(int x, int y);
float x_modf(float x, float y);
int   x_mini(int x, int y);
float x_minf(float x, float y);
int   x_maxi(int x, int y);
float x_maxf(float x, float y);
int   x_floori(float x);
float x_floorf(float x);
int   x_ceili(float x);
float x_ceilf(float x);
int   x_roundi(float x);
float x_roundf(float x);
float x_dist(float x1, float y1, float z1, float x2, float y2, float z2);
float x_nanglef(float rad);
float x_canglef(float rad);

float x_angle_diff(float rad1, float rad2);
//these two currently not working as expected
float x_ease_to_angle(float cur, float target, float percent_close, float dt);

float x_ease_to_target(float cur, float target, float percent_close, float dt);
float x_angle_to_target(float eye_x, float eye_y, float target_x, float target_y);

void  x_billboard(ScePspFVector3* out, ScePspFVector3 pos, ScePspFMatrix4* view_mat);
void  x_interpolate_points(ScePspFVector3* out, ScePspFVector3* start, ScePspFVector3* finish, float percent);
float x_magnitude(ScePspFVector3* vector);
void  x_normalize(ScePspFVector3* vector, float final_mag);
float x_dotproduct(ScePspFVector3* v1, ScePspFVector3* v2);
void  x_crossproduct(ScePspFVector3* out, ScePspFVector3* v1, ScePspFVector3* v2);
void  x_normal(ScePspFVector3* out, ScePspFVector3* p1, ScePspFVector3* p2, ScePspFVector3* p3);
void  x_translate(ScePspFVector3* vector, ScePspFVector3* trans);
void  x_rotatex(ScePspFVector3* vector, float rad);
void  x_rotatey(ScePspFVector3* vector, float rad);
void  x_rotatez(ScePspFVector3* vector, float rad);

unsigned int x_avg_col(int num_col, ... );

#ifdef __cplusplus
}
#endif

#endif
