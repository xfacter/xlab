#ifndef __X_CTRL_H__
#define __X_CTRL_H__

#include <pspctrl.h>
#include "xconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

void xCtrlInit();
u32 xCtrlUpdate(float dt);
int xCtrlPress(u32 buttons);
int xCtrlTap(u32 buttons);
int xCtrlHold(u32 buttons, float seconds);
int xCtrlAnalogAlive(float deadzone);
float xCtrlAnalogX();
float xCtrlAnalogY();

#ifdef __cplusplus
}
#endif

#endif
