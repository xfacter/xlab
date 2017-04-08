#ifndef __X_TIME_H__
#define __X_TIME_H__

#include "xconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define x_dt (xTimeGetDt())

void xTimeInit();
void xTimeUpdate();
float xTimeGetDt();
float xTimeFpsExact();
int xTimeFpsApprox();
float xTimeSecPassed();

#ifdef __cplusplus
}
#endif

#endif
