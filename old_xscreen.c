#include <pspgu.h>
#include <pspgum.h>
#include "xmem.h"
#include "xgraphics.h"
#include "xdraw.h"

#include "xscreen.h"

#define GET_8888_R(c) (((c)>>0)&0xff)
#define GET_8888_G(c) (((c)>>8)&0xff)
#define GET_8888_B(c) (((c)>>16)&0xff)
#define GET_8888_A(c) (((c)>>24)&0xff)

extern int x_buf_psm;

/*
void xScreenModify(int op, u8 red, u8 green, u8 blue)
{
    if (x_buf_psm != GU_PSM_8888) return;
    unsigned int color;
    u8 r, g, b;
    int x, y;
    for (y = 0; y < X_SCREEN_HEIGHT; y++)
    {
        for (x = 0; x < X_SCREEN_WIDTH; x++)
        {
            color = xGuGetPixel(x, y);
            r = GET_8888_R(color);
            g = GET_8888_G(color);
            b = GET_8888_B(color);
            if (op == X_SCREEN_MULTIPLY)
            {
                r = (r*red)/255;
                g = (g*green)/255;
                b = (b*blue)/255;
            }
            else if (op == X_SCREEN_ADD)
            {
                r = r+red;
                g = g+green;
                b = b+blue;
            }
            else if (op == X_SCREEN_SUBTRACT)
            {
                r = r-red;
                g = g-green;
                b = b-blue;
            }
            else if (op == X_SCREEN_REVERSE_SUBTRACT)
            {
                r = red-r;
                g = green-g;
                b = blue-b;
            }
            xGuSetPixel(x, y, X_COLOR_8888(r, g, b, 255));
        }
    }
}
*/

#define RENDER_WIDTH (64)
#define RENDER_HEIGHT (32)

void xScreenRadialBlur(int num, float scale_inc, float rot_inc, float alpha_start)
{
    void* temp_buf = xGuStridePtr(0, 1);
    xGuSaveStates();
    sceGuEnable(GU_TEXTURE_2D);
    sceGuDisable(GU_BLEND);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGB);
    sceGuTexFilter(GU_LINEAR, GU_LINEAR);
    sceGuTexMode(x_buf_psm, 0, 0, 0);
    sceGuTexImage(0, 512, 512, X_BUFFER_WIDTH, xGuDrawPtr(1, 1));
    xGuRenderTarget(x_buf_psm, RENDER_WIDTH, RENDER_HEIGHT, X_BUFFER_WIDTH, X_CACHED(X_VREL(temp_buf)));
    TVertex2D* vertices = (TVertex2D*)sceGuGetMemory(2*sizeof(TVertex2D));
    vertices[0].u = 0;
    vertices[0].v = 0;
    vertices[0].x = 0;
    vertices[0].y = 0;
    vertices[0].z = 0;
    vertices[1].u = X_SCREEN_WIDTH;
    vertices[1].v = X_SCREEN_HEIGHT;
    vertices[1].x = RENDER_WIDTH;
    vertices[1].y = RENDER_HEIGHT;
    vertices[1].z = 0;
    sceGuDrawArray(GU_SPRITES, TVertex2D_vtype|GU_TRANSFORM_2D, 2, 0, vertices);
    xGuRenderReset();
    float alpha_inc = alpha_start/num;
    sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGB);
    sceGuTexMode(x_buf_psm, 0, 0, 0);
    sceGuTexImage(0, RENDER_WIDTH, RENDER_HEIGHT, X_BUFFER_WIDTH, temp_buf);
    sceGuTexScale(1.0f, 1.0f);
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_LIGHTING);
    int started_ortho = xGuSetOrtho();
    sceGumPushMatrix();
    sceGumLoadIdentity();
    xGumTranslate(X_SCREEN_WIDTH*0.5f, X_SCREEN_HEIGHT*0.5f, 0.0f);
    int i;
    for (i = 0; i < num; i++)
    {
        sceGumRotateZ(rot_inc);
        xGumScale(1.0f+scale_inc, 1.0f+scale_inc, 0.0f);
        sceGuColor(((u8)(alpha_start*255)<<24)|0x00ffffff);
        sceGumPushMatrix();
        xGumScale((float)X_SCREEN_WIDTH, (float)X_SCREEN_HEIGHT, 0.0f);
        xGumDrawUnitTexQuad();
        sceGumPopMatrix();
        alpha_start -= alpha_inc;
    }
    sceGumPopMatrix();
    xGuLoadStates();
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    if (started_ortho) xGuSetPerspective();
}

void xScreenSimpleBlur(float alpha)
{
    void* temp_buf = xGuStridePtr(0, 1);
    xGuSaveStates();
    sceGuEnable(GU_TEXTURE_2D);
    sceGuDisable(GU_BLEND);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGB);
    sceGuTexFilter(GU_LINEAR, GU_LINEAR);
    sceGuTexMode(x_buf_psm, 0, 0, 0);
    sceGuTexImage(0, 512, 512, X_BUFFER_WIDTH, xGuDrawPtr(1, 1));
    xGuRenderTarget(x_buf_psm, RENDER_WIDTH, RENDER_HEIGHT, X_BUFFER_WIDTH, X_CACHED(X_VREL(temp_buf)));
    TVertex2D* vertices = (TVertex2D*)sceGuGetMemory(2*sizeof(TVertex2D));
    vertices[0].u = 0;
    vertices[0].v = 0;
    vertices[0].x = 0;
    vertices[0].y = 0;
    vertices[0].z = 0;
    vertices[1].u = X_SCREEN_WIDTH;
    vertices[1].v = X_SCREEN_HEIGHT;
    vertices[1].x = RENDER_WIDTH;
    vertices[1].y = RENDER_HEIGHT;
    vertices[1].z = 0;
    sceGuDrawArray(GU_SPRITES, TVertex2D_vtype|GU_TRANSFORM_2D, 2, 0, vertices);
    xGuRenderReset();
    sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGB);
    sceGuTexMode(x_buf_psm, 0, 0, 0);
    sceGuTexImage(0, RENDER_WIDTH, RENDER_HEIGHT, X_BUFFER_WIDTH, temp_buf);
    sceGuEnable(GU_BLEND);
    vertices = (TVertex2D*)sceGuGetMemory(2*sizeof(TVertex2D));
    vertices[0].u = 0;
    vertices[0].v = 0;
    vertices[0].x = 0;
    vertices[0].y = 0;
    vertices[0].z = 0;
    vertices[1].u = RENDER_WIDTH;
    vertices[1].v = RENDER_HEIGHT;
    vertices[1].x = X_SCREEN_WIDTH;
    vertices[1].y = X_SCREEN_HEIGHT;
    vertices[1].z = 0;
    sceGuColor(((u8)(alpha*255)<<24)|0x00ffffff);
    sceGuDrawArray(GU_SPRITES, TVertex2D_vtype|GU_TRANSFORM_2D, 2, 0, vertices);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    xGuLoadStates();
}

void xScreenGaussianBlur(float radius)
{
    void* temp_buf = xGuStridePtr(0, 1);
    xGuSaveStates();
    sceGuEnable(GU_TEXTURE_2D);
    sceGuDisable(GU_BLEND);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGB);
    sceGuTexFilter(GU_LINEAR, GU_LINEAR);
    sceGuTexMode(x_buf_psm, 0, 0, 0);
    sceGuTexImage(0, 512, 512, X_BUFFER_WIDTH, xGuDrawPtr(1, 1));
    xGuRenderTarget(x_buf_psm, RENDER_WIDTH, RENDER_HEIGHT, X_BUFFER_WIDTH, X_CACHED(X_VREL(temp_buf)));
    TVertex2D* vertices = (TVertex2D*)sceGuGetMemory(2*sizeof(TVertex2D));
    vertices[0].u = 0;
    vertices[0].v = 0;
    vertices[0].x = 0;
    vertices[0].y = 0;
    vertices[0].z = 0;
    vertices[1].u = X_SCREEN_WIDTH;
    vertices[1].v = X_SCREEN_HEIGHT;
    vertices[1].x = RENDER_WIDTH;
    vertices[1].y = RENDER_HEIGHT;
    vertices[1].z = 0;
    sceGuDrawArray(GU_SPRITES, TVertex2D_vtype|GU_TRANSFORM_2D, 2, 0, vertices);
    xGuRenderReset();
    float alpha_start = 0.25f;
    float alpha_inc = alpha_start/4;
    sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGB);
    sceGuTexMode(x_buf_psm, 0, 0, 0);
    sceGuTexImage(0, RENDER_WIDTH, RENDER_HEIGHT, X_BUFFER_WIDTH, temp_buf);
    sceGuTexScale(1.0f, 1.0f);
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_LIGHTING);
    int started_ortho = xGuSetOrtho();
    sceGumPushMatrix();
    sceGumLoadIdentity();
    xGumTranslate(X_SCREEN_WIDTH*0.5f, X_SCREEN_HEIGHT*0.5f, 0.0f);
    sceGuColor(((u8)(alpha_start*255)<<24)|0x00ffffff);
    xGumTranslate(-radius, -radius, 0.0f);
    sceGumPushMatrix();
    xGumScale((float)X_SCREEN_WIDTH, (float)X_SCREEN_HEIGHT, 0.0f);
    xGumDrawUnitTexQuad();
    sceGumPopMatrix();
    alpha_start -= alpha_inc;
    sceGuColor(((u8)(alpha_start*255)<<24)|0x00ffffff);
    xGumTranslate(radius, 0.0f, 0.0f);
    sceGumPushMatrix();
    xGumScale((float)X_SCREEN_WIDTH, (float)X_SCREEN_HEIGHT, 0.0f);
    xGumDrawUnitTexQuad();
    sceGumPopMatrix();
    alpha_start -= alpha_inc;
    sceGuColor(((u8)(alpha_start*255)<<24)|0x00ffffff);
    xGumTranslate(0.0f, -radius, 0.0f);
    sceGumPushMatrix();
    xGumScale((float)X_SCREEN_WIDTH, (float)X_SCREEN_HEIGHT, 0.0f);
    xGumDrawUnitTexQuad();
    sceGumPopMatrix();
    alpha_start -= alpha_inc;
    sceGuColor(((u8)(alpha_start*255)<<24)|0x00ffffff);
    xGumTranslate(-radius, 0.0f, 0.0f);
    sceGumPushMatrix();
    xGumScale((float)X_SCREEN_WIDTH, (float)X_SCREEN_HEIGHT, 0.0f);
    xGumDrawUnitTexQuad();
    sceGumPopMatrix();
    alpha_start -= alpha_inc;
    sceGumPopMatrix();
    xGuLoadStates();
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    if (started_ortho) xGuSetPerspective();
}

static u32 monochrome_color;
static u32 dichrome_colors[2];
static u32 binary_colors[2];
static u8 binary_threshold;
static u8 color_divisions;
static u32 depth_color;

static u32* grayscale_palette = 0;
static u32* night_palette = 0;
static u32* thermal_palette = 0;
static u32* monochrome_palette = 0;
static u32* dichrome_palette = 0;
static u32* binary_palette = 0;
static u32* color_reduce_palette = 0;
static u32* depth_palette = 0;

static inline void fill_grayscale_palette()
{
    u8* temp_ptr = (u8*)grayscale_palette;
    int i;
    for (i = 0; i < 256; i++)
    {
        *(temp_ptr++) = i/3;
        *(temp_ptr++) = i/3;
        *(temp_ptr++) = i/3;
        *(temp_ptr++) = 0xff;
    }
}

static inline void fill_night_palette()
{
    u8* temp_ptr = (u8*)night_palette;
    int i;
    for (i = 0; i < 256; i++)
    {
        *(temp_ptr++) = 0;
        *(temp_ptr++) = i/2;
        *(temp_ptr++) = 0;
        *(temp_ptr++) = 0xff;
    }
}

static inline void fill_thermal_palette()
{
    u8* temp_ptr = (u8*)thermal_palette;
    int i;
    for(i = 0; i < 256; i++)
    {
        if(i >= 224) {
            *(temp_ptr++) = 0xff;
            *(temp_ptr++) = (255 - ((i - 224) << 3));
            *(temp_ptr++) = 0;
        } else if(i >= 192) {
            *(temp_ptr++) = ((i - 192) << 3);
            *(temp_ptr++) = 0xff;
            *(temp_ptr++) = 0;
        } else if(i >= 160) {
            *(temp_ptr++) = 0;
            *(temp_ptr++) = 0xff;
            *(temp_ptr++) = (255 - ((i - 160) << 3));
        } else if(i >= 128) {
            *(temp_ptr++) = 0;
            *(temp_ptr++) = ((i - 128) << 3);
            *(temp_ptr++) = 0xff;
        } else {
            *(temp_ptr++) = 0;
            *(temp_ptr++) = 0;
            *(temp_ptr++) = (i << 1);
        }
        *(temp_ptr++) = 0xff;
    }
}

static inline void fill_monochrome_palette(u32 color)
{
    u8* temp_ptr = (u8*)monochrome_palette;
    int i;
    for(i = 0; i < 256; i++)
    {
        *(temp_ptr++) = (((i * (color & 0x000000ff)) >> 8) / 3);
        *(temp_ptr++) = (((i * ((color & 0x0000ff00) >> 8)) >> 8) / 3);
        *(temp_ptr++) = (((i * ((color & 0x00ff0000) >> 16)) >> 8) / 3);
        *(temp_ptr++) = 0xff;
    }
    monochrome_color = color;
}

static inline void fill_dichrome_palette(u32 light, u32 dark)
{
    u8* temp_ptr = (u8*)dichrome_palette;
    int i;
    for(i = 0; i < 256; i++)
    {
        *(temp_ptr++) = (((i * GET_8888_R(light)) + ((255 - i) * GET_8888_R(dark))) >> 9);
        *(temp_ptr++) = (((i * GET_8888_G(light)) + ((255 - i) * GET_8888_G(dark))) >> 9);
        *(temp_ptr++) = (((i * GET_8888_B(light)) + ((255 - i) * GET_8888_B(dark))) >> 9);
        *(temp_ptr++) = 0xff;
    }
    dichrome_colors[0] = light;
    dichrome_colors[1] = dark;
}

static inline void fill_binary_palette(u32 light, u32 dark, u8 threshold)
{
    u8* temp_ptr = (u8*)binary_palette;
    int i;
    for (i = 0; i < 256; i++)
    {
        if (i < threshold) {
            *(temp_ptr++) = GET_8888_R(dark);
            *(temp_ptr++) = GET_8888_G(dark);
            *(temp_ptr++) = GET_8888_B(dark);
        } else {
            *(temp_ptr++) = GET_8888_R(light);
            *(temp_ptr++) = GET_8888_G(light);
            *(temp_ptr++) = GET_8888_B(light);
        }
        *(temp_ptr++) = 0xff;
    }
    binary_colors[0] = light;
    binary_colors[1] = dark;
    binary_threshold = threshold;
}

static inline void fill_color_reduce_palette(u8 divisions)
{
    u8* temp_ptr = (u8*)color_reduce_palette;
    int i;
    for (i = 0; i < 256; i++)
    {
        *(temp_ptr++) = (i/divisions)*divisions;
        *(temp_ptr++) = 0;
        *(temp_ptr++) = 0;
        *(temp_ptr++) = 0xff;
    }
    for (i = 0; i < 256; i++)
    {
        *(temp_ptr++) = 0;
        *(temp_ptr++) = (i/divisions)*divisions;
        *(temp_ptr++) = 0;
        *(temp_ptr++) = 0xff;
    }
    for (i = 0; i < 256; i++)
    {
        *(temp_ptr++) = 0;
        *(temp_ptr++) = 0;
        *(temp_ptr++) = (i/divisions)*divisions;
        *(temp_ptr++) = 0xff;
    }
    color_divisions = divisions;
}

static inline void fill_depth_palette(u32 color)
{
    u8* temp_ptr = (u8*)depth_palette;
    int i;
    for(i = 0; i < 256; i++)
    {
        *(temp_ptr++) = GET_8888_R(color);
        *(temp_ptr++) = GET_8888_G(color);
        *(temp_ptr++) = GET_8888_B(color);
        *(temp_ptr++) = ((i * i) / 255);
    }
    depth_color = color;
}

void xScreenDestroyPalettes()
{
    if (grayscale_palette) x_free(grayscale_palette);
    if (night_palette) x_free(night_palette);
    if (thermal_palette) x_free(thermal_palette);
    if (monochrome_palette) x_free(monochrome_palette);
    if (dichrome_palette) x_free(dichrome_palette);
    if (binary_palette) x_free(binary_palette);
    if (color_reduce_palette) x_free(color_reduce_palette);
    if (depth_palette) x_free(depth_palette);
}

#define BLOCK_WIDTH (32)
#define BLOCK_HEIGHT (X_SCREEN_HEIGHT)
#define BLOCK_BUFFER_WIDTH (X_BUFFER_WIDTH)
#define BLOCK_POW2_WIDTH (32)
#define BLOCK_POW2_HEIGHT (512)

static TVertex2D __attribute__((aligned(16))) xScreenVert[2] =
{
    {0, 0, 0, 0, 0},
    {BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_WIDTH, BLOCK_HEIGHT, 0}
};

#define xScreenVert_vtype (GU_TEXTURE_16BIT|GU_VERTEX_16BIT)
#define xScreenVert_prim (GU_SPRITES)
#define xScreenVert_count (2)

static inline void x_screen_draw_section()
{
    sceGuDrawArray(xScreenVert_prim, xScreenVert_vtype|GU_TRANSFORM_2D, xScreenVert_count, 0, xScreenVert);
}

void xScreenGrayscale()
{
    if (x_buf_psm != GU_PSM_8888) return;
    if (!grayscale_palette)
    {
        grayscale_palette = (u32*)x_malloc(256*sizeof(u32));
        fill_grayscale_palette();
        sceKernelDcacheWritebackInvalidateAll();
    }
    sceGuClutLoad(32, grayscale_palette);
    sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGB);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    sceGuTexMode(GU_PSM_T32, 0, 0, 0);
    
    sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, 0x00ffffff, 0x00ffffff);

    void* draw_buf = xGuDrawPtr(1, 1);
    void* temp_buf = xGuStridePtr(0, 1);
    sceGuDrawBufferList(GU_PSM_8888, X_CACHED(X_VREL(temp_buf)), BLOCK_BUFFER_WIDTH);
    
    xGuSaveStates();
    sceGuEnable(GU_TEXTURE_2D);
    sceGuDisable(GU_DEPTH_TEST);
    int i;
    for (i = 0; i < X_SCREEN_WIDTH; i += BLOCK_WIDTH)
    {
        sceGuTexImage(0, BLOCK_POW2_WIDTH, BLOCK_POW2_HEIGHT, X_BUFFER_WIDTH, (void*)((u32)draw_buf + i*4));
        sceGuClutMode(GU_PSM_8888, 0, 0xff, 0);
        sceGuDisable(GU_BLEND);
        x_screen_draw_section();
        sceGuEnable(GU_BLEND);
        sceGuClutMode(GU_PSM_8888, 8, 0xff, 0);
        x_screen_draw_section();
        sceGuClutMode(GU_PSM_8888, 16, 0xff, 0);
        x_screen_draw_section();
        sceGuCopyImage(GU_PSM_8888, 0, 0, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_BUFFER_WIDTH, temp_buf, i, 0, X_BUFFER_WIDTH, draw_buf);
    }
    xGuLoadStates();
    
    sceGuDrawBufferList(GU_PSM_8888, draw_buf, X_BUFFER_WIDTH);
    
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
}

void xScreenNight()
{
    if (x_buf_psm != GU_PSM_8888) return;
    if (!night_palette)
    {
        night_palette = (u32*)x_malloc(256*sizeof(u32));
        fill_night_palette();
        sceKernelDcacheWritebackInvalidateAll();
    }
    sceGuClutLoad(32, night_palette);
    sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGB);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    sceGuTexMode(GU_PSM_T32, 0, 0, 0);

    sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, 0x00ffffff, 0x00ffffff);

    void* draw_buf = xGuDrawPtr(1, 1);
    void* temp_buf = (void*)((u32)xGuDrawPtr(0, 1) + 4*X_SCREEN_WIDTH);
    sceGuDrawBufferList(GU_PSM_8888, X_CACHED(X_VREL(temp_buf)), BLOCK_BUFFER_WIDTH);

    xGuSaveStates();
    sceGuEnable(GU_TEXTURE_2D);
    sceGuDisable(GU_DEPTH_TEST);
    int i;
    for (i = 0; i < X_SCREEN_WIDTH; i += BLOCK_WIDTH)
    {
        sceGuTexImage(0, BLOCK_POW2_WIDTH, BLOCK_POW2_HEIGHT, X_BUFFER_WIDTH, (void*)((u32)draw_buf + i*4));
        sceGuClutMode(GU_PSM_8888, 0, 0xff, 0);
        sceGuDisable(GU_BLEND);
        x_screen_draw_section();
        sceGuEnable(GU_BLEND);
        sceGuClutMode(GU_PSM_8888, 8, 0xff, 0);
        x_screen_draw_section();
        sceGuClutMode(GU_PSM_8888, 16, 0xff, 0);
        x_screen_draw_section();
        sceGuCopyImage(GU_PSM_8888, 0, 0, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_BUFFER_WIDTH, temp_buf, i, 0, X_BUFFER_WIDTH, draw_buf);
    }
    xGuLoadStates();

    sceGuDrawBufferList(GU_PSM_8888, draw_buf, X_BUFFER_WIDTH);

    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
}

void xScreenThermal()
{
    if (x_buf_psm != GU_PSM_8888) return;
    if (!grayscale_palette)
    {
        grayscale_palette = (u32*)x_malloc(256*sizeof(u32));
        fill_grayscale_palette();
        sceKernelDcacheWritebackInvalidateAll();
    }
    if (!thermal_palette)
    {
        thermal_palette = (u32*)x_malloc(256*sizeof(u32));
        fill_thermal_palette();
        sceKernelDcacheWritebackInvalidateAll();
    }
    sceGuClutLoad(32, grayscale_palette);
    sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGB);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    sceGuTexMode(GU_PSM_T32, 0, 0, 0);

    sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, 0x00ffffff, 0x00ffffff);

    void* draw_buf = xGuDrawPtr(1, 1);
    void* temp_buf = (void*)((u32)xGuDrawPtr(0, 1) + 4*X_SCREEN_WIDTH);
    sceGuDrawBufferList(GU_PSM_8888, X_CACHED(X_VREL(temp_buf)), BLOCK_BUFFER_WIDTH);

    xGuSaveStates();
    sceGuEnable(GU_TEXTURE_2D);
    sceGuDisable(GU_DEPTH_TEST);
    int i;
    for (i = 0; i < X_SCREEN_WIDTH; i += BLOCK_WIDTH)
    {
        sceGuTexImage(0, BLOCK_POW2_WIDTH, BLOCK_POW2_HEIGHT, X_BUFFER_WIDTH, (void*)((u32)draw_buf + i*4));
        sceGuClutMode(GU_PSM_8888, 0, 0xff, 0);
        sceGuDisable(GU_BLEND);
        x_screen_draw_section();
        sceGuEnable(GU_BLEND);
        sceGuClutMode(GU_PSM_8888, 8, 0xff, 0);
        x_screen_draw_section();
        sceGuClutMode(GU_PSM_8888, 16, 0xff, 0);
        x_screen_draw_section();
        sceGuCopyImage(GU_PSM_8888, 0, 0, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_BUFFER_WIDTH, temp_buf, i, 0, X_BUFFER_WIDTH, draw_buf);
    }
    
    sceGuClutLoad(32, thermal_palette);
    sceGuClutMode(GU_PSM_8888, 0, 0xff, 0);
    sceGuDisable(GU_BLEND);
    
    for (i = 0; i < X_SCREEN_WIDTH; i += BLOCK_WIDTH)
    {
        sceGuTexImage(0, BLOCK_POW2_WIDTH, BLOCK_POW2_HEIGHT, X_BUFFER_WIDTH, (void*)((u32)draw_buf + i*4));
        x_screen_draw_section();
        sceGuCopyImage(GU_PSM_8888, 0, 0, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_BUFFER_WIDTH, temp_buf, i, 0, X_BUFFER_WIDTH, draw_buf);
    }
    
    xGuLoadStates();

    sceGuDrawBufferList(GU_PSM_8888, draw_buf, X_BUFFER_WIDTH);

    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
}

void xScreenInverted()
{
    sceGuEnable(GU_COLOR_LOGIC_OP);
    sceGuLogicalOp(GU_INVERTED);
    Vertex2D* vertices = (Vertex2D*)sceGuGetMemory(2*sizeof(Vertex2D));
    vertices[0].x = 0;
    vertices[0].y = 0;
    vertices[0].z = 0;
    vertices[1].x = X_SCREEN_WIDTH;
    vertices[1].y = X_SCREEN_HEIGHT;
    vertices[1].z = 0;
    sceGuColor(0xffffffff);
    sceGuDrawArray(GU_SPRITES, Vertex2D_vtype|GU_TRANSFORM_2D, 2, 0, vertices);
    sceGuDisable(GU_COLOR_LOGIC_OP);
}

void xScreenMonochrome(u32 color)
{
    if (x_buf_psm != GU_PSM_8888) return;
    if (!monochrome_palette || color != monochrome_color)
    {
        if (!monochrome_palette) monochrome_palette = (u32*)x_malloc(256*sizeof(u32));
        fill_monochrome_palette(color);
        sceKernelDcacheWritebackInvalidateAll();
    }
    sceGuClutLoad(32, monochrome_palette);
    sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGB);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    sceGuTexMode(GU_PSM_T32, 0, 0, 0);

    sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, 0x00ffffff, 0x00ffffff);

    void* draw_buf = xGuDrawPtr(1, 1);
    void* temp_buf = (void*)((u32)xGuDrawPtr(0, 1) + 4*X_SCREEN_WIDTH);
    sceGuDrawBufferList(GU_PSM_8888, X_CACHED(X_VREL(temp_buf)), BLOCK_BUFFER_WIDTH);

    xGuSaveStates();
    sceGuEnable(GU_TEXTURE_2D);
    sceGuDisable(GU_DEPTH_TEST);
    int i;
    for (i = 0; i < X_SCREEN_WIDTH; i += BLOCK_WIDTH)
    {
        sceGuTexImage(0, BLOCK_POW2_WIDTH, BLOCK_POW2_HEIGHT, X_BUFFER_WIDTH, (void*)((u32)draw_buf + i*4));
        sceGuClutMode(GU_PSM_8888, 0, 0xff, 0);
        sceGuDisable(GU_BLEND);
        x_screen_draw_section();
        sceGuEnable(GU_BLEND);
        sceGuClutMode(GU_PSM_8888, 8, 0xff, 0);
        x_screen_draw_section();
        sceGuClutMode(GU_PSM_8888, 16, 0xff, 0);
        x_screen_draw_section();
        sceGuCopyImage(GU_PSM_8888, 0, 0, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_BUFFER_WIDTH, temp_buf, i, 0, X_BUFFER_WIDTH, draw_buf);
    }
    xGuLoadStates();

    sceGuDrawBufferList(GU_PSM_8888, draw_buf, X_BUFFER_WIDTH);

    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
}

void xScreenDichrome(u32 light, u32 dark)
{
    if (x_buf_psm != GU_PSM_8888) return;
    if (!grayscale_palette)
    {
        grayscale_palette = (u32*)x_malloc(256*sizeof(u32));
        fill_grayscale_palette();
        sceKernelDcacheWritebackInvalidateAll();
    }
    if (!dichrome_palette || light != dichrome_colors[0] || dark != dichrome_colors[1])
    {
        if (!dichrome_palette) dichrome_palette = (u32*)x_malloc(256*sizeof(u32));
        fill_dichrome_palette(light, dark);
        sceKernelDcacheWritebackInvalidateAll();
    }
    sceGuClutLoad(32, grayscale_palette);
    sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGB);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    sceGuTexMode(GU_PSM_T32, 0, 0, 0);

    sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, 0x00ffffff, 0x00ffffff);

    void* draw_buf = xGuDrawPtr(1, 1);
    void* temp_buf = (void*)((u32)xGuDrawPtr(0, 1) + 4*X_SCREEN_WIDTH);
    sceGuDrawBufferList(GU_PSM_8888, X_CACHED(X_VREL(temp_buf)), BLOCK_BUFFER_WIDTH);

    xGuSaveStates();
    sceGuEnable(GU_TEXTURE_2D);
    sceGuDisable(GU_DEPTH_TEST);
    int i;
    for (i = 0; i < X_SCREEN_WIDTH; i += BLOCK_WIDTH)
    {
        sceGuTexImage(0, BLOCK_POW2_WIDTH, BLOCK_POW2_HEIGHT, X_BUFFER_WIDTH, (void*)((u32)draw_buf + i*4));
        sceGuClutMode(GU_PSM_8888, 0, 0xff, 0);
        sceGuDisable(GU_BLEND);
        x_screen_draw_section();
        sceGuEnable(GU_BLEND);
        sceGuClutMode(GU_PSM_8888, 8, 0xff, 0);
        x_screen_draw_section();
        sceGuClutMode(GU_PSM_8888, 16, 0xff, 0);
        x_screen_draw_section();
        sceGuCopyImage(GU_PSM_8888, 0, 0, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_BUFFER_WIDTH, temp_buf, i, 0, X_BUFFER_WIDTH, draw_buf);
    }
    
    sceGuClutLoad(32, dichrome_palette);
    sceGuClutMode(GU_PSM_8888, 0, 0xff, 0);
    sceGuDisable(GU_BLEND);

    for (i = 0; i < X_SCREEN_WIDTH; i += BLOCK_WIDTH)
    {
        sceGuTexImage(0, BLOCK_POW2_WIDTH, BLOCK_POW2_HEIGHT, X_BUFFER_WIDTH, (void*)((u32)draw_buf + i*4));
        x_screen_draw_section();
        sceGuCopyImage(GU_PSM_8888, 0, 0, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_BUFFER_WIDTH, temp_buf, i, 0, X_BUFFER_WIDTH, draw_buf);
    }
    
    xGuLoadStates();

    sceGuDrawBufferList(GU_PSM_8888, draw_buf, X_BUFFER_WIDTH);

    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
}

void xScreenBinary(u32 light, u32 dark, u8 threshold)
{
    if (x_buf_psm != GU_PSM_8888) return;
    if (!grayscale_palette)
    {
        grayscale_palette = (u32*)x_malloc(256*sizeof(u32));
        fill_grayscale_palette();
        sceKernelDcacheWritebackInvalidateAll();
    }
    if (!binary_palette || light != binary_colors[0] || dark != binary_colors[1] || threshold != binary_threshold)
    {
        if (!binary_palette) binary_palette = (u32*)x_malloc(256*sizeof(u32));
        fill_binary_palette(light, dark, threshold);
        sceKernelDcacheWritebackInvalidateAll();
    }
    sceGuClutLoad(32, grayscale_palette);
    sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGB);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    sceGuTexMode(GU_PSM_T32, 0, 0, 0);

    sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, 0x00ffffff, 0x00ffffff);

    void* draw_buf = xGuDrawPtr(1, 1);
    void* temp_buf = (void*)((u32)xGuDrawPtr(0, 1) + 4*X_SCREEN_WIDTH);
    sceGuDrawBufferList(GU_PSM_8888, X_CACHED(X_VREL(temp_buf)), BLOCK_BUFFER_WIDTH);

    xGuSaveStates();
    sceGuEnable(GU_TEXTURE_2D);
    sceGuDisable(GU_DEPTH_TEST);
    int i;
    for (i = 0; i < X_SCREEN_WIDTH; i += BLOCK_WIDTH)
    {
        sceGuTexImage(0, BLOCK_POW2_WIDTH, BLOCK_POW2_HEIGHT, X_BUFFER_WIDTH, (void*)((u32)draw_buf + i*4));
        sceGuClutMode(GU_PSM_8888, 0, 0xff, 0);
        sceGuDisable(GU_BLEND);
        x_screen_draw_section();
        sceGuEnable(GU_BLEND);
        sceGuClutMode(GU_PSM_8888, 8, 0xff, 0);
        x_screen_draw_section();
        sceGuClutMode(GU_PSM_8888, 16, 0xff, 0);
        x_screen_draw_section();
        sceGuCopyImage(GU_PSM_8888, 0, 0, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_BUFFER_WIDTH, temp_buf, i, 0, X_BUFFER_WIDTH, draw_buf);
    }

    sceGuClutLoad(32, binary_palette);
    sceGuClutMode(GU_PSM_8888, 0, 0xff, 0);
    sceGuDisable(GU_BLEND);

    for (i = 0; i < X_SCREEN_WIDTH; i += BLOCK_WIDTH)
    {
        sceGuTexImage(0, BLOCK_POW2_WIDTH, BLOCK_POW2_HEIGHT, X_BUFFER_WIDTH, (void*)((u32)draw_buf + i*4));
        x_screen_draw_section();
        sceGuCopyImage(GU_PSM_8888, 0, 0, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_BUFFER_WIDTH, temp_buf, i, 0, X_BUFFER_WIDTH, draw_buf);
    }

    xGuLoadStates();

    sceGuDrawBufferList(GU_PSM_8888, draw_buf, X_BUFFER_WIDTH);

    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
}

void xScreenColorReduce(u8 divisions)
{
    if (x_buf_psm != GU_PSM_8888) return;
    if (!color_reduce_palette || divisions != color_divisions)
    {
        if (!color_reduce_palette) color_reduce_palette = (u32*)x_malloc(3*256*sizeof(u32));
        fill_color_reduce_palette(divisions);
        sceKernelDcacheWritebackInvalidateAll();
    }
    sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGB);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    sceGuTexMode(GU_PSM_T32, 0, 0, 0);

    sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, 0x00ffffff, 0x00ffffff);

    void* draw_buf = xGuDrawPtr(1, 1);
    void* temp_buf = (void*)((u32)xGuDrawPtr(0, 1) + 4*X_SCREEN_WIDTH);
    sceGuDrawBufferList(GU_PSM_8888, X_CACHED(X_VREL(temp_buf)), BLOCK_BUFFER_WIDTH);

    xGuSaveStates();
    sceGuEnable(GU_TEXTURE_2D);
    sceGuDisable(GU_DEPTH_TEST);
    int i;
    for (i = 0; i < X_SCREEN_WIDTH; i += BLOCK_WIDTH)
    {
        sceGuTexImage(0, BLOCK_POW2_WIDTH, BLOCK_POW2_HEIGHT, X_BUFFER_WIDTH, (void*)((u32)draw_buf + i*4));
        sceGuClutLoad(32, (void*)((u32)color_reduce_palette + 0*1024));
        sceGuClutMode(GU_PSM_8888, 0, 0xff, 0);
        sceGuDisable(GU_BLEND);
        x_screen_draw_section();
        sceGuEnable(GU_BLEND);
        sceGuClutLoad(32, (void*)((u32)color_reduce_palette + 1*1024));
        sceGuClutMode(GU_PSM_8888, 8, 0xff, 0);
        x_screen_draw_section();
        sceGuClutLoad(32, (void*)((u32)color_reduce_palette + 2*1024));
        sceGuClutMode(GU_PSM_8888, 16, 0xff, 0);
        x_screen_draw_section();
        sceGuCopyImage(GU_PSM_8888, 0, 0, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_BUFFER_WIDTH, temp_buf, i, 0, X_BUFFER_WIDTH, draw_buf);
    }
    xGuLoadStates();

    sceGuDrawBufferList(GU_PSM_8888, draw_buf, X_BUFFER_WIDTH);

    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
}

void xScreenColorEnhance()
{
    if (x_buf_psm != GU_PSM_8888) return;
    sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGB);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    sceGuTexMode(GU_PSM_8888, 0, 0, 0);

    sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, 0x00ffffff, 0x00ffffff);

    void* draw_buf = xGuDrawPtr(1, 1);
    void* temp_buf = (void*)((u32)xGuDrawPtr(0, 1) + 4*X_SCREEN_WIDTH);
    sceGuDrawBufferList(GU_PSM_8888, X_CACHED(X_VREL(temp_buf)), BLOCK_BUFFER_WIDTH);

    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuEnable(GU_TEXTURE_2D);
    sceGuDisable(GU_DEPTH_TEST);
    int i;
    for (i = 0; i < X_SCREEN_WIDTH; i += BLOCK_WIDTH)
    {
        sceGuCopyImage(GU_PSM_8888, i, 0, BLOCK_WIDTH, BLOCK_HEIGHT, X_BUFFER_WIDTH, draw_buf, 0, 0, BLOCK_BUFFER_WIDTH, temp_buf);
        sceGuTexImage(0, BLOCK_POW2_WIDTH, BLOCK_POW2_HEIGHT, X_BUFFER_WIDTH, (void*)((u32)draw_buf + i*4));
        x_screen_draw_section();
        sceGuCopyImage(GU_PSM_8888, 0, 0, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_BUFFER_WIDTH, temp_buf, i, 0, X_BUFFER_WIDTH, draw_buf);
    }
    xGuLoadStates();

    sceGuDrawBufferList(GU_PSM_8888, draw_buf, X_BUFFER_WIDTH);

    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
}

void xScreenLightEnhance()
{
    if (x_buf_psm != GU_PSM_8888) return;
    if (!grayscale_palette)
    {
        grayscale_palette = (u32*)x_malloc(256*sizeof(u32));
        fill_grayscale_palette();
        sceKernelDcacheWritebackInvalidateAll();
    }
    sceGuClutLoad(32, grayscale_palette);
    sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGB);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    sceGuTexMode(GU_PSM_T32, 0, 0, 0);

    sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, 0x00ffffff, 0x00ffffff);

    void* draw_buf = xGuDrawPtr(1, 1);
    void* temp_buf = (void*)((u32)xGuDrawPtr(0, 1) + 4*X_SCREEN_WIDTH);
    sceGuDrawBufferList(GU_PSM_8888, X_CACHED(X_VREL(temp_buf)), BLOCK_BUFFER_WIDTH);

    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuEnable(GU_TEXTURE_2D);
    sceGuDisable(GU_DEPTH_TEST);
    int i;
    for (i = 0; i < X_SCREEN_WIDTH; i += BLOCK_WIDTH)
    {
        sceGuCopyImage(GU_PSM_8888, i, 0, BLOCK_WIDTH, BLOCK_HEIGHT, X_BUFFER_WIDTH, draw_buf, 0, 0, BLOCK_BUFFER_WIDTH, temp_buf);
        sceGuTexImage(0, BLOCK_POW2_WIDTH, BLOCK_POW2_HEIGHT, X_BUFFER_WIDTH, (void*)((u32)draw_buf + i*4));
        sceGuClutMode(GU_PSM_8888, 0, 0xff, 0);
        x_screen_draw_section();
        sceGuClutMode(GU_PSM_8888, 8, 0xff, 0);
        x_screen_draw_section();
        sceGuClutMode(GU_PSM_8888, 16, 0xff, 0);
        x_screen_draw_section();
        sceGuCopyImage(GU_PSM_8888, 0, 0, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_BUFFER_WIDTH, temp_buf, i, 0, X_BUFFER_WIDTH, draw_buf);
    }
    xGuLoadStates();

    sceGuDrawBufferList(GU_PSM_8888, draw_buf, X_BUFFER_WIDTH);

    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
}

void xScreenDepthFog(u32 color, int lowbit)
{
    if (!depth_palette || color != depth_color)
    {
        if (!depth_palette) depth_palette = (u32*)x_malloc(256*sizeof(u32));
        fill_depth_palette(color);
        sceKernelDcacheWritebackInvalidateAll();
    }
    sceGuClutLoad(32, depth_palette);
    sceGuClutMode(GU_PSM_8888, (lowbit ? 0 : 8), 0xff, 0);
    sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    sceGuTexMode(GU_PSM_T16, 0, 0, 0);

    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);

    void* depth_buf = xGuDepthPtr(1, 1);

    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuEnable(GU_TEXTURE_2D);
    sceGuDisable(GU_DEPTH_TEST);
    /*
    sceGuTexImage(0, X_BUFFER_WIDTH, 512, X_BUFFER_WIDTH, depth_buf);
    TVertex2D* vertices = (TVertex2D*)sceGuGetMemory(2*sizeof(TVertex2D));
    vertices[0].u = 0;
    vertices[0].v = 0;
    vertices[0].x = 0;
    vertices[0].y = 0;
    vertices[0].z = 0;
    vertices[1].u = X_SCREEN_WIDTH;
    vertices[1].v = X_SCREEN_HEIGHT;
    vertices[1].x = X_SCREEN_WIDTH;
    vertices[1].y = X_SCREEN_HEIGHT;
    vertices[1].z = 0;
    sceGuColor(0xffffffff);
    sceGuDrawArray(GU_SPRITES, TVertex2D_vtype|GU_TRANSFORM_2D, 2, 0, vertices);
    */
    TVertex2D* vertices;
    int i;
    for (i = 0; i < X_SCREEN_WIDTH; i += BLOCK_WIDTH)
    {
        sceGuTexImage(0, BLOCK_WIDTH, 512, 512, (void*)((u32)depth_buf + i*2));
        vertices = (TVertex2D*)sceGuGetMemory(2*sizeof(TVertex2D));
        vertices[0].u = 0;
        vertices[0].v = 0;
        vertices[0].x = i;
        vertices[0].y = 0;
        vertices[0].z = 0;
        vertices[1].u = BLOCK_WIDTH;
        vertices[1].v = X_SCREEN_HEIGHT;
        vertices[1].x = i + BLOCK_WIDTH;
        vertices[1].y = X_SCREEN_HEIGHT;
        vertices[1].z = 0;
        sceGuDrawArray(GU_SPRITES, TVertex2D_vtype|GU_TRANSFORM_2D, 2, 0, vertices);
    }
    xGuLoadStates();
}
