#ifndef __X_TEXT_H__
#define __X_TEXT_H__

#include <psptypes.h>
#include "xtexture.h"

int xText2DPrintf(xTexture* bitmap, int x, int y, unsigned int color, int fw, float scale, bool aa, bool center, char* text, ... );

int xText2DPrint(xTexture* bitmap, int x, int y, unsigned int color, int fw, float scale, bool aa, bool center, char* text);

int xText2DDraw(xTexture* bitmap, int x, int y, unsigned int color, int fw, u8* fwtab, float scale, bool aa, bool center, char* text);

int xText2DLength(xTexture* bitmap, int fw, u8* fwtab, float scale, char* text);

int xText2DStringLength(xTexture* bitmap, int fw, u8* fwtab, float scale, int width, char* dest, const char* src, int num);

void xText3DDraw(xTexture* bitmap, float x, float y, float z, float length, float height, unsigned int color, int fw, char* text);

#endif
