#include "wolf_audio.h"

#include <switch.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define WOLF_AUDIO_FRAMES 1024
#define WOLF_AUDIO_BYTES (WOLF_AUDIO_FRAMES * 2 * sizeof(int16_t))
#define WOLF_AUDIO_BUFFER_SIZE 0x2000
#define WOLF_AUDIO_BUFFER_COUNT 3

static Thread audioThread;
static volatile bool audioRunning = false;
static bool audioThreadCreated = false;
static bool audioInitialized = false;

static WolfAudioCallback audioCallback = NULL;
static void *audioUserdata = NULL;

static AudioOutBuffer audioBuffers[WOLF_AUDIO_BUFFER_COUNT];
static void *audioMemory[WOLF_AUDIO_BUFFER_COUNT];

static void audioFillBuffer(AudioOutBuffer *buffer)
{
    memset(buffer->buffer, 0, WOLF_AUDIO_BYTES);

    if (audioCallback)
    {
        audioCallback(
            audioUserdata,
            (uint8_t *)buffer->buffer,
            WOLF_AUDIO_BYTES
        );
    }

    buffer->data_offset = 0;
    buffer->data_size = WOLF_AUDIO_BYTES;

    armDCacheFlush(buffer->buffer, WOLF_AUDIO_BYTES);
}

static void audioThreadMain(void *arg)
{
    (void)arg;

    // Prime all three buffers before playback begins.
    for (int i = 0; i < WOLF_AUDIO_BUFFER_COUNT; i++)
    {
        audioFillBuffer(&audioBuffers[i]);

        if (R_FAILED(audoutAppendAudioOutBuffer(&audioBuffers[i])))
        {
            audioRunning = false;
            return;
        }
    }

    // Reuse the actual buffer returned by Ounce.
    while (audioRunning)
    {
        AudioOutBuffer *released = NULL;
        u32 releasedCount = 0;

        Result rc = audoutWaitPlayFinish(
            &released,
            &releasedCount,
            AUDOUT_MAX_DELAY
        );

        if (R_FAILED(rc))
            break;

        if (!released)
            continue;

        // Refill and submit the released buffer.
        audioFillBuffer(released);

        if (R_FAILED(audoutAppendAudioOutBuffer(released)))
            break;
    }

    audioRunning = false;
}

int wolfAudioInit(WolfAudioCallback callback, void *userdata)
{
    if (audioInitialized)
        return 0;

    audioCallback = callback;
    audioUserdata = userdata;

    Result rc = audoutInitialize();

    if (R_FAILED(rc))
        return -1;

    for (int i = 0; i < WOLF_AUDIO_BUFFER_COUNT; i++)
    {
        audioMemory[i] = aligned_alloc(
            0x1000,
            WOLF_AUDIO_BUFFER_SIZE
        );

        if (!audioMemory[i])
        {
            wolfAudioShutdown();
            return -1;
        }

        memset(audioMemory[i], 0, WOLF_AUDIO_BUFFER_SIZE);
        memset(&audioBuffers[i], 0, sizeof(AudioOutBuffer));

        audioBuffers[i].next = NULL;
        audioBuffers[i].buffer = audioMemory[i];
        audioBuffers[i].buffer_size = WOLF_AUDIO_BUFFER_SIZE;
        audioBuffers[i].data_size = WOLF_AUDIO_BYTES;
        audioBuffers[i].data_offset = 0;
    }

    rc = audoutStartAudioOut();

    if (R_FAILED(rc))
    {
        wolfAudioShutdown();
        return -1;
    }

    audioRunning = true;

    rc = threadCreate(
        &audioThread,
        audioThreadMain,
        NULL,
        NULL,
        0x10000,
        0x2C,
        -2
    );

    if (R_FAILED(rc))
    {
        audioRunning = false;
        wolfAudioShutdown();
        return -1;
    }

    audioThreadCreated = true;

    rc = threadStart(&audioThread);

    if (R_FAILED(rc))
    {
        audioRunning = false;
        wolfAudioShutdown();
        return -1;
    }

    audioInitialized = true;

    return 0;
}

void wolfAudioShutdown(void)
{
    audioRunning = false;

    if (audioThreadCreated)
    {
        threadWaitForExit(&audioThread);
        threadClose(&audioThread);
        audioThreadCreated = false;
    }

    audoutStopAudioOut();

    for (int i = 0; i < WOLF_AUDIO_BUFFER_COUNT; i++)
    {
        free(audioMemory[i]);
        audioMemory[i] = NULL;
    }

    audoutExit();

    audioInitialized = false;
    audioCallback = NULL;
    audioUserdata = NULL;
}
