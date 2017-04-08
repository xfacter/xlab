#ifndef __X_TEXTURE_H__
#define __X_TEXTURE_H__

#include <psptypes.h>

#define X_MAPPED    (1)
#define X_RGBA      (2)
#define X_GRAYSCALE (3)

int NextPow2(int num);

class xTexture
{
private:
    int type;
    bool swizzled;
    bool in_vram;
    u16 buf_width;
    u16 buf_height;
    u16 width;
    u16 height;
    float x_scale;
    float y_scale;
    u8* clut;
    u8* data;
public:
    xTexture();
    ~xTexture();
    bool LoadTGA(char* filename);
    bool LoadPNG(char* filename);
    bool LoadBMP(char* filename);
    void Free();
    void SetMode(int maxmips);
    void SetImage(int maxmips, int mipmap);
    void Swizzle();
    void GrayscaleToAlpha();
    void MoveToVRAM();
    void MoveFromVRAM();
    
    int Width() {return (int)width;}
    int Height() {return (int)height;}
    bool Swizzled() {return swizzled;}
    bool Valid() {return (data ? 1 : 0);}
};

#endif
