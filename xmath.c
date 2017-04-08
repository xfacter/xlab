#include <stdarg.h>
#include <math.h>
#include <pspkernel.h>
#include <psprtc.h>
#include <psptypes.h>
#include <pspmath.h>

#include "xmath.h"

#define INLINE

/* x_math functions */

static SceKernelUtilsMt19937Context ctx;

#ifdef INLINE
inline
#endif
void x_srand(unsigned int seed)
{
    sceKernelUtilsMt19937Init(&ctx, seed);
    vfpu_srand(seed);
}

#ifdef INLINE
inline
#endif
void x_auto_srand()
{
    u64 time;
    sceRtcGetCurrentTick(&time);
    x_srand(time);
}

#ifdef INLINE
inline
#endif
int x_randi(int min, int max)
{
    if (min >= max) return min;
    return (min + sceKernelUtilsMt19937UInt(&ctx) % (max - min + 1));
}

#ifdef INLINE
inline
#endif
float x_randf(float min, float max)
{
    if (min >= max) return min;
    return vfpu_randf(min, max);
}

#ifdef INLINE
inline
#endif
float x_itof(int x)
{
    union {float f; int i;} temp;
    temp.i = x;
    return temp.f;
}

#ifdef INLINE
inline
#endif
int x_ftoi(float x)
{
    union {float f; int i;} temp;
    temp.f = x;
    return temp.i;
}

#ifdef INLINE
inline
#endif
float x_sqrtf(float x)
{
    return sqrtf(x);
    /*
    float result;
    __asm__ volatile
    (
        "mtv     %1, S000\n"
        "vsqrt.s S000, S000\n"
        "mfv     %0, S000\n"
        : "=r"(result) : "r"(x)
    );
    return result;
    */
}

#ifdef INLINE
inline
#endif
float x_inv_sqrtf(float x)
{
    /*
    float xhalf = 0.5f * x;
    int i = x_ftoi(x);
    i = 0x5f3759d5 - (i >> 1);
    x = x_itof(i);
    x = x*(1.5f - xhalf*x*x);
    return x;
    */
    float result;
    __asm__ volatile
    (
        "mtv    %1, S000\n"
        "vrsq.s S000, S000\n"
        "mfv    %0, S000\n"
        : "=r"(result) : "r"(x)
    );
    return result;
}

float x_inverse(float x)
{
    /*
    s8 exp = ((x_ftoi(x) >> 23) & 0xff) - 127;
    exp = 0 - exp;
    x = x_itof((x_ftoi(x) & 0x807FFFFF) | ((u8)(exp + 127) << 22));
    return x;
    */
    float result;
    __asm__ volatile
    (
        "mtv    %1, S000\n"
        "vrcp.s S000, S000\n"
        "mfv    %0, S000\n"
        : "=r"(result) : "r"(x)
    );
    return result;
}

#ifdef INLINE
inline
#endif
int x_even_odd(int x)
{
    return (x & 1);
}

#ifdef INLINE
inline
#endif
int x_absi(int x)
{
    return (x < 0 ? -x : x);
}

#ifdef INLINE
inline
#endif
float x_absf(float x)
{
    return fabsf(x);
}

#ifdef INLINE
inline
#endif
float x_sinf(float rad)
{
    return vfpu_sinf(rad);
}

#ifdef INLINE
inline
#endif
float x_cosf(float rad)
{
    return vfpu_cosf(rad);
}

#ifdef INLINE
inline
#endif
float x_tanf(float x)
{
    return vfpu_tanf(x);
}

#ifdef INLINE
inline
#endif
float x_asinf(float x)
{
    return vfpu_asinf(x);
}

#ifdef INLINE
inline
#endif
float x_acosf(float x)
{
    return vfpu_acosf(x);
}

#ifdef INLINE
inline
#endif
float x_atanf(float x)
{
    return vfpu_atanf(x);
}

#ifdef INLINE
inline
#endif
float x_atan2f(float y, float x)
{
    if (x == 0 && y == 0) return 0;
    //return vfpu_atan2f(y, x);
    float result;
    if (fabsf(x) >= fabsf(y))
    {
        result = vfpu_atanf(y*x_inverse(x));
        if (x < 0.0f) result += (y>=0.0f ? PI : -PI);
    }
    else
    {
        result = -vfpu_atanf(x*x_inverse(y));
        result += (y < 0.0f ? -PI_2 : PI_2);
    }
    return result;
}

#ifdef INLINE
inline
#endif
float x_sinhf(float x)
{
    return vfpu_sinhf(x);
}

#ifdef INLINE
inline
#endif
float x_coshf(float x)
{
    return vfpu_coshf(x);
}

#ifdef INLINE
inline
#endif
float x_tanhf(float x)
{
    return vfpu_tanhf(x);
}

#ifdef INLINE
inline
#endif
void x_sincos(float rad, float* sin, float* cos)
{
    vfpu_sincos(rad, sin, cos);
}

#ifdef INLINE
inline
#endif
float x_expf(float x)
{
    return vfpu_expf(x);
}

#ifdef INLINE
inline
#endif
float x_log10f(float x)
{
    float result;
    __asm__ volatile
    (
        "mtv     %1, S000\n"
        "vcst.s  S001, VFPU_LOG2TEN\n"
        "vrcp.s  S001, S001\n"
        "vlog2.s S000, S000\n"
        "vmul.s  S000, S000, S001\n"
        "mfv     %0, S000\n"
        : "=r"(result) : "r"(x)
    );
    return result;
}

#ifdef INLINE
inline
#endif
float x_logbf(float b, float x)
{
    float result;
    __asm__ volatile
    (
        "mtv     %1, S000\n"
        "mtv     %2, S001\n"
        "vlog2.s S000, S000\n"
        "vlog2.s S001, S001\n"
        "vrcp.s  S001, S001\n"
        "vmul.s  S000, S000, S001\n"
        "mfv     %0, S000\n"
        : "=r"(result) : "r"(x), "r"(b)
    );
    return result;
}

#ifdef INLINE
inline
#endif
float x_lnf(float x)
{
    float result;
    __asm__ volatile
    (
        "mtv     %1, S000\n"
        "vcst.s  S001, VFPU_LOG2E\n"
        "vrcp.s  S001, S001\n"
        "vlog2.s S000, S000\n"
        "vmul.s  S000, S000, S001\n"
        "mfv     %0, S000\n"
        : "=r"(result) : "r"(x)
    );
    return result;
}

#ifdef INLINE
inline
#endif
float x_powf(float x, float pow)
{
    return vfpu_powf(x, pow);
}

#ifdef INLINE
inline
#endif
int x_next_pow2i(int x)
{
	int pow2 = 0;
	for (pow2 = 1 << 0; pow2 < x; pow2 = pow2 << 1);
	return pow2;
}

#ifdef INLINE
inline
#endif
int x_modi(int x, int y)
{
    return x % y;
}

#ifdef INLINE
inline
#endif
float x_modf(float x, float y)
{
    return vfpu_fmodf(x, y);
}

#ifdef INLINE
inline
#endif
int x_mini(int x, int y)
{
    return (x < y ? x : y);
}

#ifdef INLINE
inline
#endif
float x_minf(float x, float y)
{
    return (x < y ? x : y);
}

#ifdef INLINE
inline
#endif
int x_maxi(int x, int y)
{
    return (x > y ? x : y);
}

#ifdef INLINE
inline
#endif
float x_maxf(float x, float y)
{
    return (x > y ? x : y);
}

#ifdef INLINE
inline
#endif
int x_floori(float x)
{
    return (int)floorf(x);
}

#ifdef INLINE
inline
#endif
float x_floorf(float x)
{
    return floorf(x);
}

#ifdef INLINE
inline
#endif
int x_ceili(float x)
{
    return (int)ceilf(x);
}

#ifdef INLINE
inline
#endif
float x_ceilf(float x)
{
    return ceilf(x);
}

#ifdef INLINE
inline
#endif
int x_roundi(float x)
{
    float normal = x - x_floorf(x);
    int a = x_floori(x);
    if (normal >= 0.5f) return a+1;
    return a;
}

#ifdef INLINE
inline
#endif
float x_roundf(float x)
{
    return (float)x_roundi(x);
}

#ifdef INLINE
inline
#endif
float x_dist(float x1, float y1, float z1, float x2, float y2, float z2)
{
    return sqrtf(SQR(x2 - x1) + SQR(y2 - y1) + SQR(z2 - z1));
}

#ifdef INLINE
inline
#endif
float x_nanglef(float rad)
{
    if (rad >= -PI && rad < PI) return rad;
    rad = vfpu_fmodf(rad, PI);
    if (rad < 0) rad += PI;
    else rad -= PI;
    return rad;
}

#ifdef INLINE
inline
#endif
float x_canglef(float rad)
{
    if (rad >= 0 && rad < TWO_PI) return rad;
    rad = vfpu_fmodf(fabsf(rad), TWO_PI);
    return rad;
}

#ifdef INLINE
inline
#endif
float x_angle_diff(float rad1, float rad2)
{
    rad1 = x_nanglef(rad1);
    rad2 = x_nanglef(rad2);
    float diff = rad2 - rad1;
    if (diff < -PI) diff += TWO_PI;
    if (diff >  PI) diff -= TWO_PI;
    return diff;
}

#ifdef INLINE
inline
#endif
float x_ease_to_angle(float cur, float target, float percent_close, float dt)
{
    float diff = x_angle_diff(cur, target);
    diff *= percent_close;
    return x_nanglef(cur + diff*dt);
}

#ifdef INLINE
inline
#endif
float x_ease_to_target(float cur, float target, float percent_close, float dt)
{
    float diff = target - cur;
    diff *= percent_close;
    return (cur + diff*dt);
}

#ifdef INLINE
inline
#endif
float x_angle_to_target(float eye_x, float eye_y, float target_x, float target_y)
{
    return x_atan2f(target_y - eye_y, target_x - eye_x);
}

#ifdef INLINE
inline
#endif
void x_billboard(ScePspFVector3* out, ScePspFVector3 pos, ScePspFMatrix4* view_mat)
{
    out->x = pos.x*view_mat->x.x + pos.y*view_mat->y.x + pos.z*view_mat->z.x + view_mat->w.x;
    out->y = pos.x*view_mat->x.y + pos.y*view_mat->y.y + pos.z*view_mat->z.y + view_mat->w.y;
    out->z = pos.x*view_mat->x.z + pos.y*view_mat->y.z + pos.z*view_mat->z.z + view_mat->w.z;
}

#ifdef INLINE
inline
#endif
void x_interpolate_points(ScePspFVector3* out, ScePspFVector3* start, ScePspFVector3* finish, float percent)
{
    ScePspFVector3 a, b;
    a = *start;
    b = *finish;
    out->x = a.x + percent*(b.x - a.x);
    out->y = a.y + percent*(b.y - a.y);
    out->z = a.z + percent*(b.z - a.z);
}

#ifdef INLINE
inline
#endif
float x_magnitude(ScePspFVector3* vector)
{
    //return sqrtf(SQR(vector->x) + SQR(vector->y) + SQR(vector->z));
    float result;
    __asm__ volatile
    (
        "lv.s   S000, 0 + %1\n"
        "lv.s   S001, 4 + %1\n"
        "lv.s   S002, 8 + %1\n"
        "vdot.t S000, C000, C000\n"
        "mfv    %0, S000\n"
        : "=r"(result) : "m"(*vector)
    );
    return sqrtf(result);
}

#ifdef INLINE
inline
#endif
void x_normalize(ScePspFVector3* vector, float final_mag)
{
    /*
    float mag = x_magnitude(vector);
    if (mag)
    {
        float inv_mag = final_mag/mag;
        vector->x *= inv_mag;
        vector->y *= inv_mag;
        vector->z *= inv_mag;
    }
    */
    __asm__ volatile
    (
        "lv.s   S000, 0 + %0\n"
        "lv.s   S001, 4 + %0\n"
        "lv.s   S002, 8 + %0\n"
        "vdot.t S010, C000, C000\n"
        "vrsq.s S010, S010\n"
        "mtv    %1, S020\n"
        "vmul.s S010, S010, S020\n"
        "vscl.t C000, C000, S010\n"
        "sv.s   S000, 0 + %0\n"
        "sv.s   S001, 4 + %0\n"
        "sv.s   S002, 8 + %0\n"
        : "+m"(*vector) : "r"(final_mag)
    );
}

#ifdef INLINE
inline
#endif
float x_dotproduct(ScePspFVector3* v1, ScePspFVector3* v2)
{
    //return (v1->x*v2->x + v1->y*v2->y + v1->z*v2->z);
    float result;
    __asm__ volatile
    (
        "lv.s   S000, 0 + %1\n"
        "lv.s   S001, 4 + %1\n"
        "lv.s   S002, 8 + %1\n"
        "lv.s   S010, 0 + %2\n"
        "lv.s   S011, 4 + %2\n"
        "lv.s   S012, 8 + %2\n"
        "vdot.t S000, C000, C010\n"
        "mfv    %0, S000\n"
        : "=r"(result) : "m"(*v1), "m"(*v2)
    );
    return result;
}

#ifdef INLINE
inline
#endif
void x_crossproduct(ScePspFVector3* out, ScePspFVector3* v1, ScePspFVector3* v2)
{
    /*
    ScePspFVector3 vert1 = *v1;
    ScePspFVector3 vert2 = *v2;
    out->x = vert1.y*vert2.z - vert1.z*vert2.y;
    out->y = vert1.z*vert2.x - vert1.x*vert2.z;
    out->z = vert1.x*vert2.y - vert1.y*vert2.x;
    */
    __asm__ volatile
    (
        "lv.s    S000, 0 + %1\n"
        "lv.s    S001, 4 + %1\n"
        "lv.s    S002, 8 + %1\n"
        "lv.s    S010, 0 + %2\n"
        "lv.s    S011, 4 + %2\n"
        "lv.s    S012, 8 + %2\n"
        "vcrsp.t C020, C000, C010\n"
        "sv.s    S020, 0 + %0\n"
        "sv.s    S021, 4 + %0\n"
        "sv.s    S022, 8 + %0\n"
        : "=m"(*out) : "m"(*v1), "m"(*v2)
    );
}

#ifdef INLINE
inline
#endif
void x_normal(ScePspFVector3* out, ScePspFVector3* p1, ScePspFVector3* p2, ScePspFVector3* p3)
{
    /*
    ScePspFVector3 v1, v2;
    
    v1.x = p1->x - p2->x;
    v1.y = p1->y - p2->y;
    v1.z = p1->z - p2->z;
    
    v2.x = p3->x - p2->x;
    v2.y = p3->y - p2->y;
    v2.z = p3->z - p2->z;
    
    x_crossproduct(out, &v1, &v2);
    x_normalize(out, 1.0f);
    */
    ScePspFVector3 v1, v2;
    v1.x = p1->x - p2->x;
    v1.y = p1->y - p2->y;
    v1.z = p1->z - p2->z;
    v2.x = p3->x - p2->x;
    v2.y = p3->y - p2->y;
    v2.z = p3->z - p2->z;
    __asm__ volatile
    (
        "lv.s    S000, 0 + %1\n"
        "lv.s    S001, 4 + %1\n"
        "lv.s    S002, 8 + %1\n"
        "lv.s    S010, 0 + %2\n"
        "lv.s    S011, 4 + %2\n"
        "lv.s    S012, 8 + %2\n"
        "vcrsp.t C020, C000, C010\n"
        "vdot.t  S000, C020, C020\n"
        "vrsq.s  S000, S000\n"
        "vscl.t  C020, C020, S000\n"
        "sv.s    S020, 0 + %0\n"
        "sv.s    S021, 4 + %0\n"
        "sv.s    S022, 8 + %0\n"
        : "=m"(*out) : "m"(v1), "m"(v2)
    );
}

#ifdef INLINE
inline
#endif
void x_translate(ScePspFVector3* vector, ScePspFVector3* trans)
{
    vector->x += trans->x;
    vector->y += trans->y;
    vector->z += trans->z;
}

#ifdef INLINE
inline
#endif
void x_rotatex(ScePspFVector3* vector, float rad)
{
    float sin, cos;
    x_sincos(rad, &sin, &cos);
    ScePspFVector3 vec = *vector;
    vector->y = (cos * vec.y) - (sin * vec.z);
    vector->z = (sin * vec.y) + (cos * vec.z);
}

#ifdef INLINE
inline
#endif
void x_rotatey(ScePspFVector3* vector, float rad)
{
    float sin, cos;
    x_sincos(rad, &sin, &cos);
    ScePspFVector3 vec = *vector;
    vector->x = (cos * vec.x) + (sin * vec.z);
    vector->z = -(sin * vec.x) + (cos * vec.z);
}

#ifdef INLINE
inline
#endif
void x_rotatez(ScePspFVector3* vector, float rad)
{
    float sin, cos;
    x_sincos(rad, &sin, &cos);
    ScePspFVector3 vec = *vector;
    vector->x = (cos * vec.x) - (sin * vec.y);
    vector->y = (sin * vec.x) + (cos * vec.y);
}

unsigned int x_avg_col(int num_col, ... )
{
    unsigned int red_sum = 0;
    unsigned int blue_sum = 0;
    unsigned int green_sum = 0;
    unsigned int alpha_sum = 0;
    unsigned int color = 0;
    va_list ap;
    va_start(ap, num_col);
    int i;
    for (i = 0; i < num_col; i++)
    {
        color = va_arg(ap, unsigned int);
        red_sum   += color >>  0 & 0xff;
        blue_sum  += color >>  8 & 0xff;
        green_sum += color >> 16 & 0xff;
        alpha_sum += color >> 24 & 0xff;
    }
    va_end(ap);
    color = (red_sum/num_col << 0) | (blue_sum/num_col << 8) | (green_sum/num_col << 16) | (alpha_sum/num_col << 24);
    return color;
}
