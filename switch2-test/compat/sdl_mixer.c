#include <SDL/SDL_mixer.h>
#include "wolf_audio.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define CHANNEL_COUNT MIX_CHANNELS

typedef struct {
    Mix_Chunk *chunk;
    unsigned position;
    int loops;
    int group;
    Uint8 left, right;
    unsigned long age;
} Channel;

static Channel channels[CHANNEL_COUNT];
static Mix_MusicCallback musicCallback;
static Mix_MusicCallback postCallback;
static void *musicUserdata;
static void *postUserdata;
static void (*finishedCallback)(int);
static unsigned long playAge;
static int reservedChannels;
static int audioStarted;

static int16_t clamp16(int value)
{
    if(value > 32767) return 32767;
    if(value < -32768) return -32768;
    return (int16_t)value;
}

static void mixerCallback(void *userdata, uint8_t *stream, int length)
{
    (void)userdata;
    memset(stream, 0, length);

    if(musicCallback)
        musicCallback(musicUserdata, stream, length);

    int16_t *samples = (int16_t *)stream;
    int frames = length / 4;

    for(int c = 0; c < CHANNEL_COUNT; c++)
    {
        Channel *ch = &channels[c];

        if(!ch->chunk)
            continue;

        for(int i = 0; i < frames; i++)
        {
            if(ch->position + 4 > ch->chunk->alen)
            {
                if(ch->loops == -1 || ch->loops-- > 0)
                    ch->position = 0;
                else
                {
                    ch->chunk = NULL;
                    if(finishedCallback)
                        finishedCallback(c);
                    break;
                }
            }

            const int16_t *src =
                (const int16_t *)(ch->chunk->abuf + ch->position);

            int left = samples[i*2] +
                (src[0] * ch->left) / 255;
            int right = samples[i*2+1] +
                (src[1] * ch->right) / 255;

            samples[i*2] = clamp16(left);
            samples[i*2+1] = clamp16(right);

            ch->position += 4;
        }
    }

    if(postCallback)
        postCallback(postUserdata, stream, length);
}

SDL_RWops *SDL_RWFromMem(void *memory, int size)
{
    SDL_RWops *rw = malloc(sizeof(*rw));
    if(!rw) return NULL;

    rw->memory = memory;
    rw->size = size;
    return rw;
}


Mix_Chunk *Mix_LoadWAV_RW(SDL_RWops *source, int freesrc)
{
    if (!source || !source->memory || source->size < 44)
        return NULL;

    const uint8_t *data = source->memory;

    unsigned offset = 12;
    unsigned dataOffset = 0;
    unsigned dataLength = 0;
    unsigned channels = 0;
    unsigned bitsPerSample = 0;
    unsigned blockAlign = 0;
    unsigned audioFormat = 0;

    while (offset + 8 <= (unsigned)source->size)
    {
        unsigned size =
            (unsigned)data[offset+4] |
            ((unsigned)data[offset+5] << 8) |
            ((unsigned)data[offset+6] << 16) |
            ((unsigned)data[offset+7] << 24);

        if (size > (unsigned)source->size - offset - 8)
            break;

        if (!memcmp(data + offset, "fmt ", 4) && size >= 16)
        {
            const uint8_t *fmt = data + offset + 8;

            audioFormat = fmt[0] | (fmt[1] << 8);
            channels = fmt[2] | (fmt[3] << 8);

            blockAlign = fmt[12] | (fmt[13] << 8);
            bitsPerSample = fmt[14] | (fmt[15] << 8);
        }
        else if (!memcmp(data + offset, "data", 4))
        {
            dataOffset = offset + 8;
            dataLength = size;
        }

        offset += 8 + size + (size & 1);
    }

    Mix_Chunk *chunk = NULL;

    if (audioFormat == 1 &&
        bitsPerSample == 16 &&
        (channels == 1 || channels == 2) &&
        blockAlign == channels * 2 &&
        dataLength > 0 &&
        dataLength % blockAlign == 0 &&
        dataOffset <= (unsigned)source->size &&
        dataLength <= (unsigned)source->size - dataOffset)
    {
        unsigned frames = dataLength / blockAlign;
        unsigned outputLength = frames * 4;

        chunk = calloc(1, sizeof(*chunk));

        if (chunk)
        {
            chunk->abuf = malloc(outputLength);

            if (chunk->abuf)
            {
                const int16_t *input =
                    (const int16_t *)(data + dataOffset);

                int16_t *output = (int16_t *)chunk->abuf;

                for (unsigned i = 0; i < frames; i++)
                {
                    int16_t left = input[i * channels];
                    int16_t right = channels == 1
                        ? left
                        : input[i * 2 + 1];

                    output[i * 2] = left;
                    output[i * 2 + 1] = right;
                }

                chunk->alen = outputLength;
                chunk->allocated = 1;
                chunk->volume = 128;
            }
            else
            {
                free(chunk);
                chunk = NULL;
            }
        }
    }

    if (freesrc)
        free(source);

    return chunk;
}


int Mix_OpenAudio(int frequency, Uint16 format,
                  int channelCount, int chunksize)
{
    (void)chunksize;

    if(frequency != 48000 || format != AUDIO_S16 ||
       channelCount != 2)
        return -1;

    if(audioStarted)
        return 0;

    memset(channels, 0, sizeof(channels));

    for(int i = 0; i < CHANNEL_COUNT; i++)
        channels[i].left = channels[i].right = 255;

    if(wolfAudioInit(mixerCallback, NULL) != 0)
        return -1;

    audioStarted = 1;
    return 0;
}

int Mix_ReserveChannels(int number)
{
    reservedChannels = number;
    return reservedChannels;
}

int Mix_GroupChannels(int from, int to, int tag)
{
    int count = 0;
    for(int i = from; i <= to && i < CHANNEL_COUNT; i++)
        if(i >= 0) {
            channels[i].group = tag;
            count++;
        }
    return count;
}

int Mix_GroupAvailable(int tag)
{
    for(int i = reservedChannels; i < CHANNEL_COUNT; i++)
        if(channels[i].group == tag && !channels[i].chunk)
            return i;
    return -1;
}

int Mix_GroupOldest(int tag)
{
    int result = -1;
    unsigned long oldest = ~0UL;

    for(int i = reservedChannels; i < CHANNEL_COUNT; i++)
        if(channels[i].group == tag && channels[i].chunk &&
           channels[i].age < oldest) {
            oldest = channels[i].age;
            result = i;
        }

    return result;
}

int Mix_SetPanning(int channel, Uint8 left, Uint8 right)
{
    if(channel < 0 || channel >= CHANNEL_COUNT)
        return 0;

    channels[channel].left = left;
    channels[channel].right = right;
    return 1;
}

int Mix_PlayChannel(int channel, Mix_Chunk *chunk, int loops)
{
    if(!chunk) return -1;

    if(channel == -1)
        for(int i = reservedChannels; i < CHANNEL_COUNT; i++)
            if(!channels[i].chunk) {
                channel = i;
                break;
            }

    if(channel < 0 || channel >= CHANNEL_COUNT)
        return -1;

    channels[channel].chunk = chunk;
    channels[channel].position = 0;
    channels[channel].loops = loops;
    channels[channel].age = ++playAge;
    return channel;
}

int Mix_HaltChannel(int channel)
{
    if(channel == -1) {
        for(int i = 0; i < CHANNEL_COUNT; i++)
            channels[i].chunk = NULL;
        return 0;
    }

    if(channel < 0 || channel >= CHANNEL_COUNT)
        return 0;

    channels[channel].chunk = NULL;
    return 0;
}

void Mix_FreeChunk(Mix_Chunk *chunk)
{
    if(!chunk) return;

    for(int i = 0; i < CHANNEL_COUNT; i++)
        if(channels[i].chunk == chunk)
            channels[i].chunk = NULL;

    if(chunk->allocated)
        free(chunk->abuf);

    free(chunk);
}

void Mix_HookMusic(Mix_MusicCallback callback, void *userdata)
{
    musicCallback = callback;
    musicUserdata = userdata;
}

void Mix_SetPostMix(Mix_MusicCallback callback, void *userdata)
{
    postCallback = callback;
    postUserdata = userdata;
}

void Mix_ChannelFinished(void (*callback)(int))
{
    finishedCallback = callback;
}

const char *Mix_GetError(void)
{
    return "Switch 2 SDL mixer error";
}
