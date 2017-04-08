#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <pspgu.h>
#include <psptypes.h>
#include "xgraphics.h"
#include "xtexture.h"

#include "xtext.h"

static u8 default_fw_tab[256] = {
	10, 10, 10, 10, 
	10, 10, 10, 10,
	10, 10, 10, 10, 
	10, 10, 10, 10,

	10, 10, 10, 10, 
	10, 10, 10, 10,
	10, 10, 10, 10,
	10, 10, 10, 10,

	10,  6,  8, 10, //   ! " #
	10, 10, 10,  6, // $ % & '
	10, 10, 10, 10, // ( ) * +
	 6, 10,  6, 10, // , - . /

	8,   6,  8,  8, // 0 1 2 3
	8,   8,  8,  8, // 6 5 8 7
	8,   8,  6,  6, // 8 9 : ;
	10, 10, 10, 10, // < = > ?

	16, 10, 10, 10, // @ A B C
	10, 10, 10, 10, // D E F G
	10,  6,  8, 10, // H I J K
	 8, 10, 10, 10, // L M N O

	10, 10, 10, 10, // P Q R S
	10, 10, 10, 12, // T U V W
	10, 10, 10, 10, // X Y Z [
	10, 10,  8, 10, // \ ] ^ _

	 6,  8,  8,  8, // ` a b c
	 8,  8,  6,  8, // d e f g
	 8,  6,  6,  8, // h i j k
	 6, 10,  8,  8, // l m n o

	 8,  8,  6,  8, // p q r s
	 6,  8,  8, 12, // t u v w
	 8,  8,  8, 10, // x y z {
	 8, 10,  8, 12, // | } ~  
	 
	10, 10, 10, 10, 
	10, 10, 10, 10,
	10, 10, 10, 10, 
	10, 10, 10, 10,

	10, 10, 10, 10, 
	10, 10, 10, 10,
	10, 10, 10, 10,
	10, 10, 10, 10,
	
	10, 10, 10, 10, 
	10, 10, 10, 10,
	10, 10, 10, 10, 
	10, 10, 10, 10,

	10, 10, 10, 10, 
	10, 10, 10, 10,
	10, 10, 10, 10,
	10, 10, 10, 10,
	
	10, 10, 10, 10, 
	10, 10, 10, 10,
	10, 10, 10, 10, 
	10, 10, 10, 10,

	10, 10, 10, 10, 
	10, 10, 10, 10,
	10, 10, 10, 10,
	10, 10, 10, 10,
	
	10, 10, 10, 10, 
	10, 10, 10, 10,
	10, 10, 10, 10, 
	10, 10, 10, 10,

	10, 10, 10, 10, 
	10, 10, 10, 10,
	10, 10, 10, 10,
	10, 10, 10, 10,
};

typedef struct {
    s16 u, v;
    s16 x, y, z;
} Text2D_Vert;

int xText2DPrintf(xTexture* bitmap, int x, int y, unsigned int color, int fw, float scale, bool aa, bool center, char* text, ... )
{
    char buffer[256];
    va_list ap;
    va_start(ap, text);
    vsnprintf(buffer, sizeof(buffer), text, ap);
    va_end(ap);
    
    return xText2DPrint(bitmap, x, y, color, fw, scale, aa, center, buffer);
}

int xText2DPrint(xTexture* bitmap, int x, int y, unsigned int color, int fw, float scale, bool aa, bool center, char* text)
{
    return xText2DDraw(bitmap, x, y, color, fw, 0, scale, aa, center, text);
}

int xText2DDraw(xTexture* bitmap, int x, int y, unsigned int color, int fw, u8* fwtab, float scale, bool aa, bool center, char* text)
{
    if (bitmap->Width() != NextPow2(bitmap->Width()) || bitmap->Height() != bitmap->Width()) return 0;
    
    int len = (int)strlen(text);
    if(len <= 0) return 0;
    
    u8 char_width = (u8)(bitmap->Width() >> 4); // divide by 16
    
    if (center)
    {
        float width = 0;
        if (fw)
        {
            width = (int)strlen(text)*fw*scale;
        }
        else
        {
            for (int i = 0; i < (int)strlen(text); i++)
            {
                unsigned char c = (unsigned char)text[i];
                u8 cw = (fwtab ? fwtab[c] : default_fw_tab[c]);
                width += cw*scale;
            }
        }
        x = (int)(x - 0.5f*width);
    }
    
    int x_start = x;
    
    Text2D_Vert* verts = (Text2D_Vert*)sceGuGetMemory((len<<1)*sizeof(Text2D_Vert));
    
    for(int i = 0; i < len; i++)
    {
        unsigned char c = (unsigned char)text[i];
        u8 cw = (fwtab ? fwtab[c] : default_fw_tab[c]);
        
        int tx = (c & 0x0F) << 4;
        int ty = (c & 0xF0);
        
        int offset = (fw ? ((char_width - fw) >> 1) : ((int)(char_width - (char_width/16.0f)*cw) >> 1));
        
        verts[(i<<1)+0].u = (s16)(tx + offset);
        verts[(i<<1)+0].v = (s16)(ty + 1);
        verts[(i<<1)+0].x = (s16)(x);
        verts[(i<<1)+0].y = (s16)(y);
        verts[(i<<1)+0].z = 0;
        
        x += (int)(scale*(fw ? fw : (int)((char_width/16.0f)*cw)));
        
        verts[(i<<1)+1].u = (s16)(tx + char_width - offset + 1);
        verts[(i<<1)+1].v = (s16)(ty + char_width + 1);
        verts[(i<<1)+1].x = (s16)(x);
        verts[(i<<1)+1].y = (s16)(y + scale*char_width);
        verts[(i<<1)+1].z = 0;
    }
    
    bitmap->SetImage(0, 0);
    sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
    xGuTexFilter(aa);
    
    xGuBlendDefault();
    
    xGuSaveStates();
    sceGuEnable(GU_TEXTURE_2D);
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuColor(color);
    sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT|GU_VERTEX_16BIT|GU_TRANSFORM_2D, len<<1, 0, verts);
    xGuLoadStates();
    
    return x - x_start;
}

int xText2DLength(xTexture* bitmap, int fw, u8* fwtab, float scale, char* text)
{
    if (bitmap->Width() != NextPow2(bitmap->Width()) || bitmap->Height() != bitmap->Width()) return 0;
    
    int len = (int)strlen(text);
    if(len <= 0) return 0;
    
    u8 char_width = (u8)(bitmap->Width() >> 4); // divide by 16
    
    int length = 0;
    for (int i = 0; i < len; i++)
    {
        unsigned char c = (unsigned char)text[i];
        u8 cw = (fwtab ? fwtab[c] : default_fw_tab[c]);
        length += (int)(scale*(fw ? fw : (int)((char_width/16.0f)*cw)));
    }
    return length;
}

int xText2DStringLength(xTexture* bitmap, int fw, u8* fwtab, float scale, int width, char* dest, const char* src, int num)
{
    if (!dest) return 0;
    if (!src || !bitmap || width <= 0 || num <= 0)
    {
        dest[0] = '\0';
        return 0;
    }
    snprintf(dest, num, src);
    for (int i = strlen(dest) - 1; i >= 0; i--)
    {
        dest[i] = '\0';
        if (xText2DLength(bitmap, fw, fwtab, scale, dest) < width)
        {
            break;
        }
    }
    return strlen(dest);
}

typedef struct {
    float u, v;
    float x, y, z;
} Text3D_Vert;

void xText3DDraw(xTexture* bitmap, float x, float y, float z, float length, float height, unsigned int color, int fw, char* text)
{
    //Reserved
}
