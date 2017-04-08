#include <string.h>
#include <pspgu.h>
#include <pspgum.h>
#include "xgraphics.h"
#include "xmath.h"
#include "xmem.h"

#include "xtexture.h"

#ifdef X_DEBUG
#include "xlog.h"
#define X_LOG(format, ... ) xLogPrintf("xTex: " format, __VA_ARGS__)
#else
#define X_LOG(format, ... ) do{}while(0)
#endif

static void swizzle_fast(void* out, const void* in, u32 width, u32 height)
{
    unsigned int blockx, blocky;
    unsigned int j;

    unsigned int width_blocks = (width / 16);
    unsigned int height_blocks = (height / 8);

    unsigned int src_pitch = (width-16)/4;
    unsigned int src_row = width * 8;

    const u8* ysrc = (u8*)in;
    u32* dst = (u32*)out;

    for (blocky = 0; blocky < height_blocks; ++blocky)
    {
        const u8* xsrc = ysrc;
        for (blockx = 0; blockx < width_blocks; ++blockx)
        {
            const u32* src = (u32*)xsrc;
            for (j = 0; j < 8; ++j)
            {
                *(dst++) = *(src++);
                *(dst++) = *(src++);
                *(dst++) = *(src++);
                *(dst++) = *(src++);
                src += src_pitch;
            }
            xsrc += 16;
        }
        ysrc += src_row;
    }
}

static int tex_psm_bpp(int psm)
{
    switch(psm)
    {
        case GU_PSM_T4:
            return 4;
        case GU_PSM_T8:
            return 8;
        case GU_PSM_5650:
        case GU_PSM_5551:
        case GU_PSM_4444:
        case GU_PSM_T16:
            return 16;
        case GU_PSM_8888:
        case GU_PSM_T32:
            return 32;
        default:
            return 0;
    }
    return 0;
}

xTexture::xTexture()
{
    clut_type = 0;
    data_type = 0;
    clut_entries = 0;
    swizzled = 0;
    in_vram = 0;
    width = 0;
    height = 0;
    buf_width = 0;
    buf_height = 0;
    pow2_height = 0;
    u_scale = 0;
    v_scale = 0;
    clut = 0;
    data = 0;
    num_mips = 0;
    mipmaps = 0;
}

xTexture::~xTexture()
{
    Free();
}

void xTexture::Swizzle()
{
    if (!data || swizzled) return;
	void* temp_swizzle = x_malloc(tex_psm_bpp(data_type)*buf_width*buf_height/8);
	if (!temp_swizzle) return;
    swizzle_fast(temp_swizzle, data, tex_psm_bpp(data_type)*buf_width/8, buf_height);
    x_free(data);
	data = temp_swizzle;
	in_vram = 0;
    int i;
    for (i = 0; i < num_mips; i++)
    {
        temp_swizzle = x_malloc(tex_psm_bpp(data_type)*mipmaps[i].buf_width*mipmaps[i].buf_height/8);
        swizzle_fast(temp_swizzle, mipmaps[i].data, tex_psm_bpp(data_type)*mipmaps[i].buf_width/8, mipmaps[i].buf_height);
        x_free(mipmaps[i].data);
        mipmaps[i].data = temp_swizzle;
        mipmaps[i].in_vram = 0;
    }
	swizzled = 1;
}

void xTexture::GenerateMipmaps(int num)
{
    //do not make mipmaps if trying to make more than 9 or texture is larger than 512x512
    //do not make mipmaps if texture is indexed (temp?)
    if (!data || num <= 0 || num > 9 || width > 512 || height > 512 || clut) return;
    FreeMipmaps();
    mipmaps = (mipmap_data*)x_malloc(num*sizeof(mipmap_data));
    if (!mipmaps) return;
    int mip_width = width;
    int mip_height = height;
    int i;
    for (i = 0; i < num; i++)
    {
        mip_width >>= 1;
        mip_height >>= 1;
        mipmaps[i].buf_width = x_next_pow2(mip_width);
        mipmaps[i].pow2_height = x_next_pow2(mip_height);
        mipmaps[i].buf_height = x_num_align(mip_height, 8);
        mipmaps[i].data = x_malloc(tex_psm_bpp(data_type)*mipmaps[i].buf_width*mipmaps[i].buf_height/8);
        if (!mipmaps[i].data) return;
        //if (clut) sceGuClutMode(clut_type, 0, 0xff, 0);
        #if 1
        xGuTexScaleToDest(data_type, width, height, buf_width, data, mip_width, mip_height, mipmaps[i].buf_width, mipmaps[i].data);
        #else
        if (i == 0) xGuTexScaleToDest(data_type, width, height, buf_width, data, mip_width, mip_height, mipmaps[i].buf_width, mipmaps[i].data);
        else xGuTexScaleToDest(data_type, mip_width << 1, mip_height << 1, mipmaps[i-1].buf_width, mipmaps[i-1].data, mip_width, mip_height, mipmaps[i].buf_width, mipmaps[i].data);
        #endif
        num_mips += 1;
    }
}

void xTexture::FreeMipmaps()
{
    if (mipmaps)
    {
        int i;
        for (i = 0; i < num_mips; i++) X_DELETE(mipmaps[i].data);
    }
    X_DELETE(mipmaps);
    num_mips = 0;
}

bool xTexture::LoadTex(char* filename, int levels, int flags)
{
    char* ptr = strrchr(filename, '.');
    if (!ptr) return false;
    
    bool r = false;
    if (stricmp(".tga", ptr) == 0)
    {
        r = LoadTGA(filename, levels, flags);
    }
    else if (stricmp(".png", ptr) == 0)
    {
        r = LoadPNG(filename, levels, flags);
    }
    else if (stricmp(".bmp", ptr) == 0)
    {
        r = LoadBMP(filename, levels, flags);
    }
    return r;
}

#ifdef X_TEX_TGA

#define TGA_NO_DATA    (0)
#define TGA_MAPPED     (1)
#define TGA_RGB        (2)
#define TGA_GRAYSCALE  (3)
#define TGA_RLE_MAPPED (9)
#define TGA_RLE_RGBA   (10)
#define TGA_COMPRESSED_GRAYSCALE (11)
#define TGA_COMPRESSED_MAPPED    (32)
#define TGA_COMPRESSED_QUADTREE  (33)

#define TGA_SHIFT(n) (1<<(n))
#define TGA_DESC_YFLIP (TGA_SHIFT(5))

u32 convert_tga_8888(u32 color)
{
    u8 r = (color >> 16) & 0xff;
    u8 b = (color >> 0) & 0xff;
    color &= 0xff00ff00;
    color |= (r << 0) | (b << 16);
    return color;
}

u16 convert_tga_5551(u16 color)
{
    u8 r = (color>>10) & 0x1f;
    u8 b = (color>>0) & 0x1f;
    color &= 0x83e0;
    color |= (1<<15)|(r<<0)|(b<<10);
    return color;
}

typedef struct
{
    u8 idlength;
    u8 colormaptype;
    u8 datatype;
    u16 colormapstart;
    u16 colormaplength;
    u8 colormapdepth;
    u16 x_origin;
    u16 y_origin;
    u16 width;
    u16 height;
    u8 bitsperpixel;
    u8 descriptor;
} __attribute__((packed)) tga_header;

bool xTexture::LoadTGA(char* filename, int levels, int flags)
{
    Free();
    FILE* file = fopen(filename, "rb");
    if (!file) return false;
    
    tga_header header;
    fread(&header, sizeof(tga_header), 1, file);
    
    width = header.width;
    height = header.height;
    buf_width = x_next_pow2(width);
    pow2_height = x_next_pow2(height);
    buf_height = x_num_align(height, 8);
    u_scale = (float)width/buf_width;
    v_scale = (float)height/pow2_height;
    
    fseek(file, header.idlength, SEEK_CUR);
    fseek(file, header.colormaplength, SEEK_CUR);
    int x, y;
    if (header.datatype == TGA_NO_DATA)
    {
        fclose(file);
        return false;
    }
    else if (header.datatype == TGA_MAPPED)
    {
        if (header.colormapdepth == 16) clut_type = GU_PSM_5551;
        else if (header.colormapdepth == 32) clut_type = GU_PSM_8888;
        if (header.bitsperpixel == 8) data_type = GU_PSM_T8;
        else if (header.bitsperpixel == 16) data_type = GU_PSM_T16;
        else if (header.bitsperpixel == 32) data_type = GU_PSM_T32;
        else
        {
            fclose(file);
            Free();
            return false;
        }
        clut_entries = tex_psm_bpp(clut_type)*header.colormaplength/8;
        clut = x_malloc(header.colormaplength);
        if (!clut)
        {
            fclose(file);
            Free();
            return false;
        }
        data = x_malloc(tex_psm_bpp(data_type)*buf_width*buf_height/8);
        if (!data)
        {
            fclose(file);
            Free();
            return false;
        }
        fseek(file, header.colormapstart, SEEK_SET);
        fread(clut, header.colormaplength, 1, file);
        for (y = 0; y < height; y++)
        {
            fread((void*)((u32)data + tex_psm_bpp(data_type)*(header.descriptor & TGA_DESC_YFLIP ? height-y-1 : y)*buf_width/8), tex_psm_bpp(data_type)*width/8, 1, file);
        }
        if (clut_type == GU_PSM_5551)
        {
            u16* ptr = (u16*)clut;
            for (x = 0; x < clut_entries; x++)
            {
                ptr[x] = convert_tga_5551(ptr[x]);
            }
        }
        else if (clut_type == GU_PSM_8888)
        {
            u32* ptr = (u32*)clut;
            for (x = 0; x < clut_entries; x++)
            {
                ptr[x] = convert_tga_8888(ptr[x]);
            }
        }
    }
    else if (header.datatype == TGA_RGB)
    {
        if (header.bitsperpixel == 16) data_type = GU_PSM_5551;
        else if (header.bitsperpixel == 24) data_type = GU_PSM_8888; //see below
        else if (header.bitsperpixel == 32) data_type = GU_PSM_8888;
        
        data = x_malloc(tex_psm_bpp(data_type)*buf_width*buf_height/8);
        if (!data)
        {
            fclose(file);
            Free();
            return false;
        }
        if (header.bitsperpixel == 24)
        {
            for (y = 0; y < height; y++)
            {
                for (x = 0; x < width; x++)
                {
                    u32* ptr = (u32*)data;
                    fread(&ptr[(header.descriptor & TGA_DESC_YFLIP ? height-y-1 : y)*buf_width + x], 3, 1, file);
                    ptr[(header.descriptor & TGA_DESC_YFLIP ? height-y-1 : y)*buf_width + x] |= (0xff << 24);
                }
            }
        }
        else
        {
            for (y = 0; y < height; y++)
            {
                fread((void*)((u32)data + tex_psm_bpp(data_type)*(header.descriptor & TGA_DESC_YFLIP ? height-y-1 : y)*buf_width/8), tex_psm_bpp(data_type)*width/8, 1, file);
            }
        }
        if (data_type == GU_PSM_5551)
        {
            u16* ptr = (u16*)data;
            for (y = 0; y < height; y++)
            {
                for (x = 0; x < width; x++)
                {
                    ptr[y*buf_width + x] = convert_tga_5551(ptr[y*buf_width + x]);
                }
            }
        }
        else if (data_type == GU_PSM_8888)
        {
            u32* ptr = (u32*)data;
            for (y = 0; y < height; y++)
            {
                for (x = 0; x < width; x++)
                {
                    ptr[y*buf_width + x] = convert_tga_8888(ptr[y*buf_width + x]);
                }
            }
        }
    }
    else if (header.datatype == TGA_GRAYSCALE)
    {
        if (header.bitsperpixel != 8)
        {
            fclose(file);
            Free();
            return false;
        }
        clut_type = GU_PSM_8888;
        data_type = GU_PSM_T8;
        clut_entries = 256;
        clut = x_malloc(256*sizeof(unsigned int));
        data = x_malloc(buf_width*buf_height);
        u32* ptr = (u32*)clut;
        for (x = 0; x < 256; x++)
        {
            if (flags & X_TEX_GRAY_TO_ALPHA) ptr[x] = GU_RGBA(255,255,255,x);
            else ptr[x] = GU_RGBA(x,x,x,255);
        }
        for (y = 0; y < height; y++)
        {
            fread((void*)((u32)data + (header.descriptor & TGA_DESC_YFLIP ? height-y-1 : y)*buf_width), width, 1, file);
        }
    }
    else
    {
        //Unsupported type
        X_LOG("Attempting to load unsupported type.", 0);
    }
    fclose(file);
    GenerateMipmaps(levels);
    Swizzle();
    if (flags & X_TEX_TOP_IN_VRAM) MoveToVRAM(0);
    if (flags & X_TEX_MIPS_IN_VRAM)
    {
        for (x = 0; x < num_mips; x++)
        {
            MoveToVRAM(x);
        }
    }
    return true;
}

#else

bool xTexture::LoadTGA(char* filename, int levels, int flags)
{
    Free();
    return false;
}

#endif

#ifdef X_TEX_PNG

#include <png.h>

static void user_warning_fn(png_structp png_ptr, png_const_charp warning_msg)
{
    //Reserved
}

bool Texture::LoadPNG(char* filename, int levels, int flags)
{
    Free();
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	png_uint_32 png_width, png_height;
	int bit_depth, color_type, interlace_type;
	u32* line;

	FILE* file = fopen(filename, "rb");
	if (!file) return false;

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
    buf_width = x_next_pow2(width);
    pow2_height = x_next_pow2(height);
    buf_height = x_num_align(height, 8);

    u_scale = (float)width/buf_width;
    v_scale = (float)height/pow2_height;

	png_set_strip_16(png_ptr);
	png_set_packing(png_ptr);
	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_gray_1_2_4_to_8(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);

	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
	
	data_type = GU_PSM_8888;
	clut_type = 0;
	data = x_malloc(buf_width*buf_height*4);
    if (!data)
    {
        fclose(file);
        return false;
    }

	line = (u32*)x_malloc(width * 4);
	u32* ptr = (u32*)data;
	int x, y;
	for (y = 0; y < height; y++)
    {
		png_read_row(png_ptr, (u8*) line, png_bytep_NULL);
		for (x = 0; x < width; x++)
        {
            ptr[(height-y-1)*buf_width + x] = line[x];
		}
	}
	x_free(line);
	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
	fclose(file);
    GenerateMipmaps(levels);
    Swizzle();
    if (flags & X_TEX_TOP_IN_VRAM) MoveToVRAM(0);
    if (flags & X_TEX_MIPS_IN_VRAM)
    {
        for (x = 0; x < num_mips; x++)
        {
            MoveToVRAM(x);
        }
    }
	return true;
}

#else

bool xTexture::LoadPNG(char* filename, int levels, int flags)
{
    Free();
    return false;
}

#endif

#ifdef X_TEX_BMP

#define BMP_IDENT (('B'<<0)|('M'<<8)) /* "BM" (0x4d42) */

typedef struct
{
    u16 id;
    u32 filesize;
    u16 reserved0;
    u16 reserved1;
    u32 datastart;
    u32 headerlength;
    u32 width;
    u32 height;
    u16 colorplanes;
    u16 bitsperpixel;
    u32 compression;
    u32 datasize;
    u32 xresolution;
    u32 yresolution;
    u32 colors;
    u32 importantcolors;
} __attribute__((packed)) bmp_header;

bool xTexture::LoadBMP(char* filename, int levels, int flags)
{
    /*
    Free();
    FILE* file = fopen(filename, "rb");
    if (!file) return false;

    bmp_header header;
    fread(&header, sizeof(bmp_header), 1, file);
    if (header.id != BMP_IDENT)
    {
        fclose(file);
        return false;
    }

    width = header.width;
    height = header.height;
    buf_width = x_next_pow2(width);
    buf_height = x_next_pow2(height);
    u_scale = (float)width/buf_width;
    v_scale = (float)height/buf_height;

    fseek(file, header.idlength, SEEK_CUR);
    fseek(file, header.colormaplength, SEEK_CUR);
    int x, y;
    int bytes;
    if (header.datatype == TGA_NO_DATA)
    {
        fclose(file);
        return false;
    }
    else if (header.datatype == TGA_MAPPED)
    {
        //
    }
    
    //...
    
    if (flags & X_TEX_SWIZZLE) Swizzle();
    if (flags & X_TEX_PUT_IN_VRAM) MoveToVRAM();
    return true;
    */
    return false;
}

#else

bool xTexture::LoadBMP(char* filename, int levels, int flags)
{
    Free();
    return false;
}

#endif

void xTexture::Free()
{
    X_DELETE(data);
    X_DELETE(clut);
    FreeMipmaps();
}

void xTexture::MoveToVRAM(int level)
{
    if (!data || level < 0) return;
    int size = 0;
    void* vram_ptr = 0;
    if (level == 0)
    {
        if (in_vram) return;
        size = tex_psm_bpp(data_type)*buf_width*buf_height/8;
        vram_ptr = x_valloc(size);
        if (!vram_ptr) return;
        memcpy(X_UNCACHED(vram_ptr), data, size);
        x_free(data);
        data = vram_ptr;
        in_vram = 1;
    }
    else
    {
        if (mipmaps[level-1].in_vram) return;
        size = tex_psm_bpp(data_type)*mipmaps[level-1].buf_width*mipmaps[level-1].buf_height/8;
        vram_ptr = x_valloc(size);
        if (!vram_ptr) return;
        memcpy(X_UNCACHED(vram_ptr), mipmaps[level-1].data, size);
        x_free(mipmaps[level-1].data);
        mipmaps[level-1].data = vram_ptr;
        mipmaps[level-1].in_vram = 1;
    }
}

void xTexture::MoveFromVRAM(int level)
{
    if (!data || level < 0) return;
    int size = 0;
    void* mem_ptr = 0;
    if (level == 0)
    {
        if (!in_vram) return;
        size = tex_psm_bpp(data_type)*buf_width*buf_height/8;
        mem_ptr = x_malloc(size);
        if (!mem_ptr) return;
        memcpy(mem_ptr, X_UNCACHED(data), size);
        x_free(data);
        data = mem_ptr;
        in_vram = 0;
    }
    else
    {
        if (!mipmaps[level-1].in_vram) return;
        size = tex_psm_bpp(data_type)*mipmaps[level-1].buf_width*mipmaps[level-1].buf_height/8;
        mem_ptr = x_malloc(size);
        if (!mem_ptr) return;
        memcpy(mem_ptr, X_UNCACHED(mipmaps[level-1].data), size);
        x_free(mipmaps[level-1].data);
        mipmaps[level-1].data = mem_ptr;
        mipmaps[level-1].in_vram = 0;
    }
}

void xTexture::SetImage()
{
    if (!data)
    {
        xGuSetDebugTex();
    }
    else
    {
        if (clut)
        {
            sceGuClutMode(clut_type, 0, 0xff, 0);
            sceGuClutLoad(clut_entries>>3, clut);
        }
        sceGuTexScale(u_scale, v_scale);
        sceGuTexMode(data_type, num_mips, 0, swizzled);
        sceGuTexImage(0, buf_width, pow2_height, buf_width, data);
        if (mipmaps)
        {
            int i;
            for (i = 0; i < num_mips; i++)
            {
                sceGuTexImage(i+1, mipmaps[i].buf_width, mipmaps[i].pow2_height, mipmaps[i].buf_width, mipmaps[i].data);
            }
        }
    }
}

/* slice texture to maximize texture cache */
#define TEX_SLICE (64)

void xTexDrawRectSect(xTexture* texture, int x, int y, int width, int height, int sx, int sy, int sw, int sh)
{
    if (!texture) return;
    xGuTexMode(GU_TFX_REPLACE, 1, 1);
    texture->SetImage();
    xGuSaveStates();
    sceGuEnable(GU_BLEND);
    sceGuEnable(GU_TEXTURE_2D);
    sceGuDisable(GU_DEPTH_TEST);
    float cur_u = (float)sx;
    float ustep = (float)TEX_SLICE*sw/width;
    int slice_width = 0;
    float tex_step = 0;
    TVertex2D* vertices = 0;
    int i;
    for (i = x; i < x + width; i += TEX_SLICE)
    {
        slice_width = (i + TEX_SLICE > x + width ? x + width - i : TEX_SLICE);
        tex_step = (cur_u + ustep > sx + sw ? sx + sw - cur_u : ustep);
        vertices = (TVertex2D*)sceGuGetMemory(2*sizeof(TVertex2D));
        vertices[0].u = (s16)cur_u;
        vertices[0].v = sy + sh;
        vertices[0].x = i;
        vertices[0].y = y;
        vertices[0].z = 0;
        cur_u += tex_step;
        vertices[1].u = (s16)cur_u;
        vertices[1].v = sy;
        vertices[1].x = i + slice_width;
        vertices[1].y = y + height;
        vertices[1].z = 0;
        sceGuDrawArray(GU_SPRITES, TVertex2D_vtype|GU_TRANSFORM_2D, 2, 0, vertices);
    }
    xGuLoadStates();
}

void xTexDrawSect(xTexture* texture, int x, int y, int sx, int sy, int sw, int sh)
{
    xTexDrawRectSect(texture, x, y, sw, sh, sx, sy, sw, sh);
}

void xTexDrawRect(xTexture* texture, int x, int y, int width, int height)
{
    xTexDrawRectSect(texture, x, y, width, height, 0, 0, texture->Width(), texture->Height());
}

void xTexDraw(xTexture* texture, int x, int y)
{
    xTexDrawRect(texture, x, y, texture->Width(), texture->Height());
}

void xTexDrawOrtho(xTexture* texture)
{
    if (!texture) return;
    xGuTexMode(GU_TFX_REPLACE, 1, 0);
    texture->SetImage();
    xGuSaveStates();
    sceGuEnable(GU_TEXTURE_2D);
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_LIGHTING);
    sceGuDisable(GU_DEPTH_TEST);
    xGumDrawUnitTexQuad();
    xGuLoadStates();
}

void xTexDrawOrthoScale(xTexture* texture)
{
    if (!texture) return;
    sceGumPushMatrix();
    xGumScale((float)texture->Width(), -(float)texture->Height(), 0.0f);
    xTexDrawOrtho(texture);
    sceGumPopMatrix();
}

void xTexDrawOrthoOrient(xTexture* texture, int x, int y, float rot)
{
    if (!texture) return;
    sceGumPushMatrix();
    sceGumLoadIdentity();
    xGumTranslate((float)x, (float)y, 0.0f);
    xGumScale((float)texture->Width(), -(float)texture->Height(), 0.0f);
    sceGumRotateZ(rot);
    xTexDrawOrtho(texture);
    sceGumPopMatrix();
}

void xTexDraw3DSect(xTexture* texture, ScePspFVector3* pos, float length, float height, int sx, int sy, int sw, int sh)
{
    if (!texture) return;
    ScePspFMatrix4 view_mat;
    sceGumMatrixMode(GU_VIEW);
    sceGumStoreMatrix(&view_mat);
    sceGumLoadIdentity();

    ScePspFVector3 translate;
    x_billboard(&translate, pos, &view_mat);
    sceGumMatrixMode(GU_MODEL);
    sceGumPushMatrix();
    sceGumLoadIdentity();
    sceGumTranslate(&translate);
    xGumScale(length, height, 1.0f);

    xGuTexMode(GU_TFX_REPLACE, 1, 1);
    texture->SetImage();
    float u0 = (float)sx / texture->Width();
    float u1 = (float)(sx+sw) / texture->Width();
    float v0 = (float)sy / texture->Height();
    float v1 = (float)(sy+sh) / texture->Height();

    xGuSaveStates();
    sceGuEnable(GU_TEXTURE_2D);
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_LIGHTING);
    TVertexF* vertices = (TVertexF*)sceGuGetMemory(4*sizeof(TVertexF));
    vertices[0].u = u0;
    vertices[0].v = v0;
    vertices[0].x = -0.5f;
    vertices[0].y = -0.5f;
    vertices[0].z = 0.0f;
    vertices[1].u = u1;
    vertices[1].v = v1;
    vertices[1].x = 0.5f;
    vertices[1].y = 0.5f;
    vertices[1].z = 0.0f;
    sceGumDrawArray(GU_SPRITES, TVertexF_vtype|GU_TRANSFORM_3D, 2, 0, vertices);
    xGuLoadStates();

    sceGumPopMatrix();
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadMatrix(&view_mat);
    sceGumMatrixMode(GU_MODEL);
}

void xTexDraw3D(xTexture* texture, ScePspFVector3* pos, float length, float height)
{
    if (!texture) return;
    ScePspFMatrix4 view_mat;
    sceGumMatrixMode(GU_VIEW);
    sceGumStoreMatrix(&view_mat);
    sceGumLoadIdentity();

    ScePspFVector3 translate;
    x_billboard(&translate, pos, &view_mat);
    sceGumMatrixMode(GU_MODEL);
    sceGumPushMatrix();
    sceGumLoadIdentity();
    sceGumTranslate(&translate);
    xGumScale(length, height, 1.0f);

    xGuTexMode(GU_TFX_REPLACE, 1, 1);
    texture->SetImage();

    xGuSaveStates();
    sceGuEnable(GU_TEXTURE_2D);
    sceGuEnable(GU_BLEND);
    sceGuDisable(GU_LIGHTING);
    xGumDrawUnitTexQuad();
    xGuLoadStates();

    sceGumPopMatrix();
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadMatrix(&view_mat);
    sceGumMatrixMode(GU_MODEL);
}
