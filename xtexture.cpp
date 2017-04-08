#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pspkernel.h>
#include <pspgu.h>
#include <psptypes.h>
#include <vram.h>

#include "xtexture.h"

int NextPow2(int num)
{
	int pow2 = 0;
	for (pow2 = 1 << 0; pow2 < num; pow2 = pow2 << 1);
	return pow2;
}

xTexture::xTexture()
{
    swizzled = 0;
    in_vram = 0;
    buf_width = 0;
    buf_height = 0;
    x_scale = 0;
    y_scale = 0;
    data = 0;
}

xTexture::~xTexture()
{
    Free();
}

#ifdef LOAD_TGA

#define TGA_MAPPED    (1)
#define TGA_RGBA      (2)
#define TGA_GRAYSCALE (3)

#define TGA_Header_size (18)

#define TGA_New_Header(name) u8 name[TGA_Header_size]

#define TGA_idlength(header)       (header[0])
#define TGA_colormaptype(header)   (header[1])
#define TGA_datatype(header)       (header[2])
#define TGA_colormapstart(header)  (header[3] | header[4] << 8)
#define TGA_colormaplength(header) (header[5] | header[6] << 8)
#define TGA_colormapbits(header)   (header[7])
#define TGA_xorigin(header)        (header[8] | header[9] << 8)
#define TGA_yorigin(header)        (header[10] | header[11] << 8)
#define TGA_width(header)          (header[12] | header[13] << 8)
#define TGA_height(header)         (header[14] | header[15] << 8)
#define TGA_pixelbits(header)      (header[16])
#define TGA_descriptor(header)     (header[17])

bool xTexture::LoadTGA(char* filename)
{
    FILE* file;
    
    file = fopen(filename, "rb");
    if (!file) return false;
    
    Free();
    
    TGA_New_Header(header);
    
    fread(header, 1, TGA_Header_size, file);
    
    //temporary checks?
    if (TGA_datatype(header) != TGA_RGBA || TGA_pixelbits(header) != 32 ||
        TGA_xorigin(header) != 0 /*|| TGA_yorigin(header) != 0*/)
    {
        fclose(file);
        return false;
    }
    
    type = TGA_datatype(header);

    width = TGA_width(header);
    height = TGA_height(header);
    
    buf_width = NextPow2(width);
    buf_height = NextPow2(height);
    x_scale = (float)width/buf_width;
    y_scale = (float)height/buf_height;
    
    fseek(file, TGA_idlength(header), SEEK_CUR);
    
    if (type == TGA_RGBA)
    {
        data = (u8*)malloc(buf_width*buf_height*4*sizeof(u8));
        
        if (!data) return false;
        
        for (u16 y = 0; y < height; y++)
        {
            fread((void*)((u32)data + 4*y*buf_width), 1, 4*width, file);
        }
        
        fclose(file);
        
        for (u32 y = 0; y < height; y++)
        {
            for (u32 x = 0; x < width; x++)
            {
                int index = 4*(y*buf_width + x);
                u8 temp = data[index];
                data[index] = data[index+2];
                data[index+2] = temp;
            }
        }
    }
    
    return true;
}

#endif

#ifdef LOAD_PNG

#include <png.h>

static void user_warning_fn(png_structp png_ptr, png_const_charp warning_msg)
{
    //Reserved
}

bool Texture::LoadPNG(char* filename)
{
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	png_uint_32 png_width, png_height;
	int bit_depth, color_type, interlace_type;
	u32* line;

	FILE* file;

	if ((file = fopen(filename, "rb")) == 0) return false;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == 0)
    {
		fclose(file);
		return false;
	}

	png_set_error_fn(png_ptr, (png_voidp) NULL, (png_error_ptr) NULL, user_warning_fn);

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
    {
		fclose(file);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return false;
	}
	png_init_io(png_ptr, file);
	png_set_sig_bytes(png_ptr, sig_read);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &png_width, &png_height, &bit_depth, &color_type, &interlace_type, int_p_NULL, int_p_NULL);

	width = png_width;
	height = png_height;
	buf_width = NextPow2(png_width);
	buf_height = NextPow2(png_height);

    x_scale = (float)width/buf_width;
    y_scale = (float)height/buf_height;

	png_set_strip_16(png_ptr);
	png_set_packing(png_ptr);
	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_gray_1_2_4_to_8(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);

	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);

	data = (u8*)calloc(buf_width*buf_height*4, sizeof(u8));

	line = (u32*) malloc(width * 4);

	for (u32 y = 0; y < height; y++)
    {
		png_read_row(png_ptr, (u8*) line, png_bytep_NULL);
		for (u32 x = 0; x < width; x++)
        {
			int index = 4*((height-y-1)*buf_width + x);
			data[index+0] = line[x] >> 0  & 0xFF;
			data[index+1] = line[x] >> 8  & 0xFF;
			data[index+2] = line[x] >> 16 & 0xFF;
			data[index+3] = line[x] >> 24 & 0xFF;
		}
	}
	free(line);
	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
	fclose(file);
	return true;
}

#endif

void xTexture::Free()
{
    if (!data) return;
    if (in_vram) vfree(data);
    else free(data);
}

static u32 debug_texture[16] =
{
    0xffff00ff, 0xff000000, 0xffff00ff, 0xff000000,
    0xff000000, 0xffff00ff, 0xff000000, 0xffff00ff,
    0xffff00ff, 0xff000000, 0xffff00ff, 0xff000000,
    0xff000000, 0xffff00ff, 0xff000000, 0xffff00ff
};

void xTexture::SetMode(int maxmips)
{
    if (type == X_RGBA) sceGuTexMode(GU_PSM_8888, maxmips, 0, swizzled);
}

void xTexture::SetImage(int maxmips, int mipmap)
{
    if (!Valid())
    {
        sceGuTexMode(GU_PSM_8888, 0, 0, 0);
        sceGuTexScale(1.0f, 1.0f);
        sceGuTexWrap(GU_REPEAT, GU_REPEAT);
        sceGuTexImage(0, 4, 4, 4, debug_texture);
    }
    else
    {
        SetMode(maxmips);
        sceGuTexScale(x_scale, y_scale);
        sceGuTexImage(mipmap, buf_width, buf_height, buf_width, (void*)data);
    }
}

void xTexture::Swizzle()
{
    if (!data) return;
    
    u32 rowblocks = buf_width*sizeof(u32) / 16;
	
	u8* out = (u8*)malloc(buf_width*buf_height*4*sizeof(u8));
	
	if (!out) return;
	
	for (u32 j = 0; j < buf_height; ++j)
	{
		for (u32 i = 0; i < (u32)(buf_width*4); ++i)
		{
			u32 blockx = i / 16;
			u32 blocky = j / 8;
	 
			u32 x = (i - blockx*16);
			u32 y = (j - blocky*8);
			u32 block_index = blockx + ((blocky) * rowblocks);
			u32 block_address = block_index * 16 * 8;
	 
			out[block_address + x + y * 16] = data[i+j*buf_width*4];
		}
	}
	Free();
	data = out;
	swizzled = !swizzled;
}

void xTexture::GrayscaleToAlpha()
{
    if (!data) return;
    
    if (type != X_RGBA) return;
    for (u32 y = 0; y < height; y++)
    {
        for (u32 x = 0; x < width; x++)
        {
            if (x < width)
            {
                u32 index = 4*(y*buf_width + x);
                data[index+3] = data[index+0];
                
                data[index+0] = 0xFF;
                data[index+1] = 0xFF;
                data[index+2] = 0xFF;
            }
        }
    }
}

void xTexture::MoveToVRAM()
{
    if (!data || in_vram) return;
    if (type != X_RGBA) return;
    
    int size = buf_width*buf_height*4;
    u8* vram_ptr = (u8*)valloc(size*sizeof(u8));
    
    if (!vram_ptr) return;
    
    memcpy(vram_ptr, data, size);
    Free();
    data = vram_ptr;
    in_vram = 1;
}

void xTexture::MoveFromVRAM()
{
    if (!data || !in_vram) return;
    if (type != X_RGBA) return;
    
    int size = buf_width*buf_height*4;
    u8* new_data = (u8*)malloc(size*sizeof(u8));
    
    if (!new_data) return;
    
    memcpy(new_data, data, size);
    Free();
    data = new_data;
    in_vram = 0;
}
