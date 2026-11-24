#include "wolf_video.h"
#include <switch.h>

#define DISPLAY_WIDTH 1280
#define DISPLAY_HEIGHT 720

static Framebuffer fb;

int wolfVideoInit(void)
{
    NWindow *win = nwindowGetDefault();

    Result rc = framebufferCreate(
        &fb, win, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        PIXEL_FORMAT_RGBA_8888, 2
    );

    if (R_FAILED(rc))
        return -1;

    rc = framebufferMakeLinear(&fb);

    if (R_FAILED(rc)) {
        framebufferClose(&fb);
        return -1;
    }

    return 0;
}

void wolfVideoPresent(const uint8_t *indexedPixels,
                      int width, int height, int pitch,
                      const WolfColor *palette)
{
    if (!indexedPixels || !palette ||
        width <= 0 || height <= 0 || pitch < width)
        return;

    u32 stride = 0;
    uint8_t *pixels = framebufferBegin(&fb, &stride);

    if (!pixels)
        return;

    // Preserve the original game's aspect ratio.
    // A 320x200 game image becomes 1152x720.
    int drawWidth = DISPLAY_WIDTH;
    int drawHeight = DISPLAY_WIDTH * height / width;

    if (drawHeight > DISPLAY_HEIGHT) {
        drawHeight = DISPLAY_HEIGHT;
        drawWidth = DISPLAY_HEIGHT * width / height;
    }

    int offsetX = (DISPLAY_WIDTH - drawWidth) / 2;
    int offsetY = (DISPLAY_HEIGHT - drawHeight) / 2;

    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        uint32_t *row = (uint32_t *)(pixels + y * stride);

        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            uint32_t color = RGBA8(0, 0, 0, 255);

            if (x >= offsetX && x < offsetX + drawWidth &&
                y >= offsetY && y < offsetY + drawHeight) {

                int sx = (x - offsetX) * width / drawWidth;
                int sy = (y - offsetY) * height / drawHeight;

                uint8_t index = indexedPixels[sy * pitch + sx];
                WolfColor c = palette[index];

                color = RGBA8(c.r, c.g, c.b, 255);
            }

            row[x] = color;
        }
    }

    framebufferEnd(&fb);
}

void wolfVideoShutdown(void)
{
    framebufferClose(&fb);
}
