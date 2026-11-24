#ifndef WOLF_VIDEO_H
#define WOLF_VIDEO_H

#include <stdint.h>

typedef struct {
    uint8_t r, g, b, unused;
} WolfColor;

int wolfVideoInit(void);
void wolfVideoPresent(const uint8_t *indexedPixels,
                      int width, int height, int pitch,
                      const WolfColor *palette);
void wolfVideoShutdown(void);

#endif
