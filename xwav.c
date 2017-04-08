#include <pspaudiolib.h>
#include <pspaudio.h>
#include "xmem.h"

#include "xwav.h"

typedef struct {
    u32 playptr;
    u32 playptr_fraction;
    u32 rateratio; //used for pitch
    float pan;
    float volume;
    u16 playing;
    u16 loop;
    xWav* wav_play;
} xWavSound;

static xWavSound X_WAV_sounds[X_WAV_SOUND_MAX];

static void X_WAV_Sound_Callback(void *buf, unsigned int reqn, void *pdata)
{
    u32 fraction;
    s16* buffer = buf;
    xWavSound* sound;
    xWav* wav;
    int i, slot;
    int out_right, out_left;
    int index;
    for (i = 0; i < reqn; i++)
    {
        out_right = 0;
        out_left = 0;
        for (slot = 0; slot < X_WAV_SOUND_MAX; slot++)
        {
            sound = &X_WAV_sounds[slot];
            wav = sound->wav_play;

            if (!sound->playing) continue;
            
            fraction = sound->playptr_fraction + sound->rateratio;
            sound->playptr += fraction >> 16;
            sound->playptr_fraction = fraction & 0xffff;
            if (sound->playptr >= wav->samplecount)
            {
                if (sound->loop)
                {
                    sound->playptr = 0;
                    sound->playptr_fraction = 0;
                }
                else
                {
                    xWavStop(slot);
                    break;
                }
            }
            s16* data = (s16*)(wav->data);
            index = wav->channels * sound->playptr;
            out_left  += (int)(data[index+0] * sound->volume * (1.0f - sound->pan));
            out_right += (int)(data[index+1] * sound->volume * sound->pan);
        }
        if (out_left < -32768) out_left = -32768;
        else if (out_left > 32767) out_left = 32767;
        if (out_right < -32768) out_right = -32768;
        else if (out_right > 32767) out_right = 32767;
        *(buffer++) = out_left;
        *(buffer++) = out_right;
    }
}

void xWavInit()
{
    pspAudioInit();
    pspAudioSetChannelCallback(X_WAV_CHANNEL, X_WAV_Sound_Callback, 0);
    pspAudioSetVolume(X_WAV_CHANNEL, PSP_VOLUME_MAX, PSP_VOLUME_MAX);
    xWavStopAll();
}

void xWavEnd()
{
    pspAudioEnd();
}

typedef struct {
    u32 ChunkID;
    u32 ChunkSize;
    u32 Format;
    u32 Subchunk1ID;
    u32 Subchunk1Size;
    u16 AudioFormat;
    u16 NumChannels;
    u32 SampleRate;
    u32 ByteRate;
    u16 BlockAlign;
    u16 BitsPerSample;
    u32 Subchunk2ID;
    u32 Subchunk2Size;
} X_WAV_header;

#define WAV_HEAD_RIFF   ('R'<<0|'I'<<8|'F'<<16|'F'<<24) /* "RIFF" (0x46464952) */
#define WAV_HEAD_FORMAT ('W'<<0|'A'<<8|'V'<<16|'E'<<24) /* "WAVE" (0x20746d66) */
#define WAV_HEAD_SUB1ID ('f'<<0|'m'<<8|'t'<<16|' '<<24) /* "fmt " (0x45564157) */
#define WAV_HEAD_SUB2ID ('d'<<0|'a'<<8|'t'<<16|'a'<<24) /* "data" (0x61746164) */

#define WAV_PCM (1)

int xWavLoad(xWav* wav, char* filename)
{
    if (!wav) return 0;
    FILE* file = fopen(filename, "rb");
    if (!file) return 0;
    
    X_WAV_header header;
    fread(&header, sizeof(X_WAV_header), 1, file);
    
    if (header.ChunkID != WAV_HEAD_RIFF || header.Format != WAV_HEAD_FORMAT ||
        header.Subchunk1ID != WAV_HEAD_SUB1ID || header.Subchunk2ID != WAV_HEAD_SUB2ID ||
        header.AudioFormat != WAV_PCM)
    {
        fclose(file);
        return 0;
    }
    
    wav->channels = header.NumChannels;
    wav->samplerate = header.SampleRate;
    wav->datalength = header.Subchunk2Size;
    wav->samplecount = wav->datalength/(wav->channels*2);
    
    wav->data = x_malloc(wav->datalength);
    if (!wav->data) return 0;
    fread(wav->data, wav->datalength, 1, file);
    fclose(file);
    
    return 1;
}

void xWavFree(xWav* wav)
{
    if (!wav) return;
    if (wav->data) x_free(wav->data);
}

int xWavPlayControl(xWav* wav, float volume, float pan, float pitch, int loop)
{
    if (!wav) return -1;
    int slot = -1;
    int i;
    for (i = 0; i < X_WAV_SOUND_MAX; i++)
    {
        if (!X_WAV_sounds[i].playing)
        {
            slot = i;
            X_WAV_sounds[slot].wav_play = wav;
            X_WAV_sounds[slot].playing = 1;
            X_WAV_sounds[slot].loop = (loop ? 1 : 0);
            X_WAV_sounds[slot].playptr = 0;
            X_WAV_sounds[slot].playptr_fraction = 0;
            xWavSetVolume(slot, volume);
            xWavSetPan(slot, pan);
            xWavSetPitch(slot, pitch);
            break;
        }
    }
    return slot;
}

int xWavPlay(xWav* wav, int loop)
{
    return xWavPlayControl(wav, 1.0f, 0.0f, 1.0f, 0);
}

int xWavStop(int slot)
{
    if (slot < 0 || slot >= X_WAV_SOUND_MAX || !X_WAV_sounds[slot].playing) return 0;
    X_WAV_sounds[slot].playing = 0;
    return 1;
}

int xWavStopAll()
{
    int stopped = 0;
    int i;
    for (i = 0; i < X_WAV_SOUND_MAX; i++)
    {
        if (xWavStop(i)) stopped += 1;
    }
    return stopped;
}

void xWavSetVolume(int slot, float volume)
{
    if (slot < 0 || slot >= X_WAV_SOUND_MAX || !X_WAV_sounds[slot].playing) return;
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    X_WAV_sounds[slot].volume = volume;
}

void xWavSetPan(int slot, float pan)
{
    if (slot < 0 || slot >= X_WAV_SOUND_MAX || !X_WAV_sounds[slot].playing) return;
    if (pan < -1.0f) pan = -1.0f;
    if (pan >  1.0f) pan =  1.0f;
    pan = (pan + 1.0f)*0.5f;
    X_WAV_sounds[slot].pan = pan;
}

void xWavSetPitch(int slot, float pitch)
{
    if (slot < 0 || slot >= X_WAV_SOUND_MAX || !X_WAV_sounds[slot].playing) return;
    if (pitch < 0.0f) pitch = 0.0f;
    xWav* wav = X_WAV_sounds[slot].wav_play;\
    u32 rateratio = (u32)(pitch*((wav->samplerate*0x4000)/11025));
    if (rateratio < (2000*0x4000)/11025) rateratio = (2000*0x4000)/11025;
    if (rateratio > (100000*0x4000)/11025) rateratio = (100000*0x4000)/11025;
    X_WAV_sounds[slot].rateratio = rateratio;
}
