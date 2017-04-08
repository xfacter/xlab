#ifndef __X_TEXTURE_H__
#define __X_TEXTURE_H__

#include "xconfig.h"

#define X_TEX_TOP_IN_VRAM   (1<<0) /* top level in vram */
#define X_TEX_MIPS_IN_VRAM  (1<<1) /* all mipmaps in vram */
#define X_TEX_ALL_IN_VRAM   (X_TEX_TOP_IN_VRAM|X_TEX_MIPS_IN_VRAM) /* all levels in vram */
#define X_TEX_GRAY_TO_ALPHA (1<<2)

typedef struct {
    u16 buf_width;
    u16 buf_height;
    u16 pow2_height;
    bool in_vram;
    void* data;
} mipmap_data;

class xTexture
{
private:
    int clut_type;
    int data_type;
    int clut_entries;
    bool swizzled;
    bool in_vram;
    u16 width;
    u16 height;
    u16 buf_width;
    u16 buf_height;
    u16 pow2_height;
    float u_scale;
    float v_scale;
    void* clut;
    void* data;
    int num_mips;
    mipmap_data* mipmaps;
    
    void Swizzle();
    void GenerateMipmaps(int num);
    void FreeMipmaps();
public:
    xTexture();
    ~xTexture();
    bool LoadTex(char* filename, int levels, int flags);
    bool LoadTGA(char* filename, int levels, int flags);
    bool LoadPNG(char* filename, int levels, int flags);
    bool LoadBMP(char* filename, int levels, int flags);
    void Free();
    void MoveToVRAM(int level);
    void MoveFromVRAM(int level);
    void SetImage();
    
    int Width() {return (int)width;}
    int Height() {return (int)height;}
};

void xTexDrawRectSect(xTexture* texture, int x, int y, int width, int height, int sx, int sy, int sw, int sh);

void xTexDrawSect(xTexture* texture, int x, int y, int sx, int sy, int sw, int sh);

void xTexDrawRect(xTexture* texture, int x, int y, int width, int height);

void xTexDraw(xTexture* texture, int x, int y);

void xTexDrawOrtho(xTexture* texture);

void xTexDrawOrthoScale(xTexture* texture);

void xTexDrawOrthoOrient(xTexture* texture, int x, int y, float rot);

void xTexDraw3DSect(xTexture* texture, ScePspFVector3* pos, float length, float height, int sx, int sy, int sw, int sh);

void xTexDraw3D(xTexture* texture, ScePspFVector3* pos, float length, float height);

#if 0
/* C-style texture loader to replace C++ style loader */
//...

#endif

#endif
