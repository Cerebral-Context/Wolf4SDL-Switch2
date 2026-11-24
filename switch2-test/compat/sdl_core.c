#include <SDL/SDL.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

int SDL_Init(Uint32 flags)
{
    (void)flags;
    return 0;
}

void SDL_Quit(void)
{
}

const char *SDL_GetError(void)
{
    return "Switch 2 SDL compatibility layer";
}

Uint32 SDL_MapRGB(SDL_PixelFormat *format,
                  Uint8 r, Uint8 g, Uint8 b)
{
    if (!format)
        return 0;

    if (format->palette && format->palette->colors)
    {
        int best = 0;
        unsigned bestDistance = ~0u;

        for (int i = 0; i < format->palette->ncolors; i++)
        {
            SDL_Color *c = &format->palette->colors[i];

            int dr = (int)r - c->r;
            int dg = (int)g - c->g;
            int db = (int)b - c->b;

            unsigned distance = dr * dr + dg * dg + db * db;

            if (distance < bestDistance)
            {
                bestDistance = distance;
                best = i;
            }
        }

        return (Uint32)best;
    }

    return ((Uint32)r << 16) |
           ((Uint32)g << 8) |
           (Uint32)b;
}

int SDL_SaveBMP(SDL_Surface *surface, const char *file)
{
    (void)surface;
    (void)file;

    /* Screenshot saving not yet implemented. */
    return -1;
}
