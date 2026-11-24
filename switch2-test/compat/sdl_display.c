#include <stddef.h>
#include <SDL/SDL.h>
#include "../wolf_video.h"

extern void wolfSetPhysicalScreen(SDL_Surface *surface);

int SDL_Flip(SDL_Surface *screen)
{
    if (!screen || !screen->pixels ||
        !screen->format || !screen->format->palette)
        return -1;

    wolfVideoPresent(
        (const uint8_t *)screen->pixels,
        screen->w,
        screen->h,
        screen->pitch,
        (const WolfColor *)screen->format->palette->colors
    );

    return 0;
}

SDL_Surface *SDL_SetVideoMode(int width, int height,
                              int bpp, Uint32 flags)
{
    static int videoInitialized = 0;

    if (!videoInitialized)
    {
        if (wolfVideoInit() != 0)
            return NULL;

        videoInitialized = 1;
    }

    if (bpp == 0)
        bpp = 8;

    SDL_Surface *surface = SDL_CreateRGBSurface(
        flags, width, height, bpp, 0, 0, 0, 0
    );

    wolfSetPhysicalScreen(surface);
    return surface;
}
