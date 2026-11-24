#ifndef WOLF_SWITCH2_AUDIO_H
#define WOLF_SWITCH2_AUDIO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*WolfAudioCallback)(
    void *userdata,
    uint8_t *stream,
    int length
);

int wolfAudioInit(WolfAudioCallback callback, void *userdata);
void wolfAudioShutdown(void);

#ifdef __cplusplus
}
#endif

#endif
