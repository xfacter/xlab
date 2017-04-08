#ifndef __X_WAV_H__
#define __X_WAV_H__

#include "xconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define X_WAV_CHANNEL   (0)
#define X_WAV_SOUND_MAX (32)

typedef struct
{
    u32 channels;
    u32 samplerate;
    u32 samplecount;
    u32 datalength;
    void* data;
} xWav;

void xWavInit();

void xWavEnd();

int xWavLoad(xWav* wav, char* filename);

void xWavFree(xWav* wav);

int xWavPlayControl(xWav* wav, float volume, float pan, float pitch, int loop);

int xWavPlay(xWav* wav, int loop);

int xWavStop(int slot);

int xWavStopAll();

/* volume should be a float from 0.0f to 1.0f */
void xWavSetVolume(int slot, float volume);

/* pan should be a float from -1.0f to 1.0f */
void xWavSetPan(int slot, float pan);

/* pitch should be a float >= 0.0f, 1.0f = normal */
void xWavSetPitch(int slot, float pitch);

#ifdef __cplusplus
}
#endif

#endif
