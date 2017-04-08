#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pspkernel.h>
#include <psptypes.h>
#include <psprtc.h>
#include <pspiofilemgr.h>
#include <pspctrl.h>

#include "xlib.h"

/* Code for setting up HOME button callbacks. */

static bool x_running = 1;

bool xLibRunning()
{
    return x_running;
}

static int xLibExitCallback(int arg1, int arg2, void *common)
{
    x_running = 0;
    return 0;
}

static int xLibCallbackThread(SceSize args, void *argp)
{
    int cbid;
    cbid = sceKernelCreateCallback("xLibExitCallback", xLibExitCallback, 0);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

int xLibSetupCallbacks()
{
    int thid = 0;
    thid = sceKernelCreateThread("xLibCallbackThread", xLibCallbackThread, 0x11, 0xFA0, PSP_THREAD_ATTR_USER, 0);
    if(thid >= 0) sceKernelStartThread(thid, 0, 0);
    x_running = 1;
    return thid;
}

/* Useful time code */

u64 xLibTime()
{
    u64 time;
    sceRtcGetCurrentTick(&time);
    return time;
}

static u64 time_cur;
static u64 time_last;
static float inv_tick_res;

void xLibDtInit()
{
    time_last = xLibTime();
    inv_tick_res = 1.0f/sceRtcGetTickResolution();
}

float xLibDtGet()
{
    time_cur = xLibTime();
    float delta_time = (time_cur - time_last)*inv_tick_res;
    time_last = time_cur;
    return delta_time;
}

float xLibDtFps(float dt)
{
    return 1.0f/dt;
}

static SceCtrlData control_cur;
static SceCtrlData control_last;

void xCtrlInit()
{
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
}

u32 xCtrlRead(float dt)
{
    control_last = control_cur;
    sceCtrlPeekBufferPositive(&control_cur, 1);
    return control_cur.Buttons;
}

static bool ctrlPress(SceCtrlData* ctrl_data, unsigned int buttons)
{
    return ((ctrl_data->Buttons & buttons) == buttons);
}

bool xCtrlPress(unsigned int buttons)
{
    return ctrlPress(&control_cur, buttons);
}

bool xCtrlTap(unsigned int buttons)
{
    return (ctrlPress(&control_cur, buttons) && !ctrlPress(&control_last, buttons));
}

bool xCtrlHold(unsigned int buttons, float seconds)
{
    //Reserved
    return false;
}

float xCtrlAnalogX()
{
    return (control_cur.Lx - 127.5f);
}

float xCtrlAnalogY()
{
    return -(control_cur.Ly - 127.5f);
}

xLibDirectory::xLibDirectory(u8 max_files, u8 max_name_length)
{
    num_files = max_files;
    name_length = max_name_length;
    files_in_dir = 0;
    
    file_names = (char**)malloc(num_files*sizeof(char*));
    for (int i = 0; i < num_files; i++) file_names[i] = (char*)malloc(name_length*sizeof(char));
}

xLibDirectory::~xLibDirectory()
{
    for (int i = 0; i < num_files; i++) free(file_names[i]);
    free(file_names);
}

void xLibDirectory::Read(const char* dir_name, const char* suffix)
{
    snprintf(directory_name, sizeof(directory_name), "%s", dir_name);
    files_in_dir = 0;
    SceIoDirent dir;
    int dfd = 0;
    memset(&dir, 0, sizeof(dir));
    dfd = sceIoDopen(dir_name);
    if (dfd > 0)
    {
        while(files_in_dir < num_files && sceIoDread(dfd, &dir) > 0)
        {
            if (!suffix || (strrchr(dir.d_name, '.') > 0 && !stricmp(suffix, strrchr(dir.d_name, '.'))))
            {
                snprintf(file_names[files_in_dir], name_length, "%s", dir.d_name);
                files_in_dir += 1;
            }
        }
    }
    sceIoDclose(dfd);
}

int xLibDirectory::NumFiles()
{
    return files_in_dir;
}

char* xLibDirectory::operator[](int index)
{
    if (index < 0 || index >= num_files) return 0;
    return file_names[index];
}

void xLibDirectory::GetFullName(int index, char* out, int max_len)
{
    if (index < 0 || index >= num_files) return;
    snprintf(out, max_len, "%s%s", directory_name, file_names[index]);
}
