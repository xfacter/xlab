#ifndef __X_LIB_H__
#define __X_LIB_H__

#include <psptypes.h>
#include <pspctrl.h>

/* Memory addresses */
#define __SCRATCH_PAD   (0x00010000)
#define __VRAM_MEM      (0x04000000)
#define __MAIN_MEM      (0x08800000)
#define __NO_CACHE      (0x40000000)
#define __VRAM_NO_CACHE (__VRAM_MEM | __NO_CACHE)
#define __MAIN_NO_CACHE (__MAIN_MEM | __NO_CACHE)
#define __KERNEL_MEM    (0x88000000)

bool xLibRunning();
int xLibSetupCallbacks();

u64 xLibTime();
void xLibDtInit();
float xLibDtGet();
float xLibDtFps(float dt);

void xCtrlInit();
u32 xCtrlRead(float dt);
bool xCtrlPress(unsigned int buttons);
bool xCtrlTap(unsigned int buttons);
bool xCtrlHold(unsigned int buttons, float seconds);
float xCtrlAnalogX();
float xCtrlAnalogY();

class xLibDirectory
{
private:
    u8 num_files;
    u8 name_length;
    u8 files_in_dir;
    char directory_name[256];
    char** file_names;
public:
    xLibDirectory(u8 max_files, u8 max_name_length);
    ~xLibDirectory();
    void Read(const char* dir_name, const char* suffix);
    int NumFiles();
    char* operator[](int index);
    void GetFullName(int index, char* out, int max_len);
};

#endif
