#ifndef WOLF_SWITCH2_SDL_MIXER_H
#define WOLF_SWITCH2_SDL_MIXER_H

#include "SDL.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_S16 0x8010
#define MIX_CHANNELS 8

typedef struct SDL_RWops {
    void *memory;
    int size;
} SDL_RWops;

typedef struct Mix_Chunk {
    int allocated;
    Uint8 *abuf;
    Uint32 alen;
    Uint8 volume;
} Mix_Chunk;

typedef void (*Mix_MusicCallback)(
    void *userdata,
    Uint8 *stream,
    int length
);

SDL_RWops *SDL_RWFromMem(void *memory, int size);

int Mix_OpenAudio(
    int frequency,
    Uint16 format,
    int channels,
    int chunksize
);

int Mix_ReserveChannels(int number);
int Mix_GroupChannels(int from, int to, int tag);
int Mix_GroupAvailable(int tag);
int Mix_GroupOldest(int tag);

int Mix_SetPanning(
    int channel,
    Uint8 left,
    Uint8 right
);

Mix_Chunk *Mix_LoadWAV_RW(
    SDL_RWops *source,
    int freesrc
);

int Mix_PlayChannel(
    int channel,
    Mix_Chunk *chunk,
    int loops
);

int Mix_HaltChannel(int channel);

void Mix_FreeChunk(Mix_Chunk *chunk);

void Mix_HookMusic(
    Mix_MusicCallback callback,
    void *userdata
);

void Mix_SetPostMix(
    Mix_MusicCallback callback,
    void *userdata
);

void Mix_ChannelFinished(
    void (*callback)(int channel)
);

const char *Mix_GetError(void);

#ifdef __cplusplus
}
#endif

#endif
