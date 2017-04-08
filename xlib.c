#include <pspsdk.h>

#include "xlib.h"

#ifdef X_DEBUG
#include "xlog.h"
#define X_LOG(format, ... ) xLogPrintf("xLib: " format, __VA_ARGS__)
#else
#define X_LOG(format, ... ) do{}while(0)
#endif

#ifdef X_LIB_KERNEL
PSP_MODULE_INFO("xLibApp", PSP_MODULE_KERNEL, 1, 1);
PSP_MAIN_THREAD_ATTR(0);
#else
PSP_MODULE_INFO("xLibApp", PSP_MODULE_USER, 1, 1);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER|PSP_THREAD_ATTR_VFPU);
#endif

static int x_running = 1;

inline int xRunning()
{
	return (int)x_running;
}

inline void xExit()
{
    x_running = 0;
}

static int xExitCallback(int arg1, int arg2, void *common)
{
	x_running = 0;
	return 0;
}

static int xCallbackThread(SceSize args, void *argp)
{
    int cbid = sceKernelCreateCallback("xExitCallback", xExitCallback, 0);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

static int xSetupCallbacks()
{
    int thid = sceKernelCreateThread("xCallbackThread", xCallbackThread, 0x11, 0xFA0, PSP_THREAD_ATTR_USER, 0);
    if(thid >= 0) sceKernelStartThread(thid, 0, 0);
    x_running = 1;
    return thid;
}

#ifdef X_LIB_KERNEL
static int xUserThread(SceSize args, void* argp)
{
    remove("./xlog.txt");
    X_LOG("Logging started.",0);
    xMain();
    X_LOG("Logging finished.",0);
    sceKernelExitGame();
    return 0;
}

int main(int argc, char *argv[])
{
    #ifdef X_MODULE_WIFI
    pspSdkLoadInetModules();
    pspSdkInetInit();
    #endif
    #ifdef X_MODULE_MP3
    //Reserved
    #endif
    xSetupCallbacks();
    int user_thid = sceKernelCreateThread("xUserThread", xUserThread, 0x18, 0x10000, PSP_THREAD_ATTR_USER|PSP_THREAD_ATTR_VFPU, 0);
    if (user_thid >= 0) sceKernelStartThread(user_thid, 0, 0);
    sceKernelExitDeleteThread(0);
    return 0;
}
#else
int main(int argc, char *argv[])
{
    xSetupCallbacks();
    remove("./xlog.txt");
    X_LOG("Logging started.",0);
    xMain();
    X_LOG("Logging finished.",0);
    sceKernelExitGame();
    return 0;
}
#endif
