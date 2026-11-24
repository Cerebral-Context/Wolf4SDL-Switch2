#include <SDL/SDL.h>
#include <stdlib.h>
#include <string.h>

static SDL_Surface *physicalScreen = NULL;

SDL_Surface *SDL_CreateRGBSurface(
    Uint32 flags, int width, int height, int depth,
    Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    if (width <= 0 || height <= 0 || depth != 8)
        return NULL;

    SDL_Surface *surface = calloc(1, sizeof(SDL_Surface));
    SDL_PixelFormat *format = calloc(1, sizeof(SDL_PixelFormat));
    SDL_Palette *palette = calloc(1, sizeof(SDL_Palette));

    if (!surface || !format || !palette)
        goto fail;

    palette->colors = calloc(256, sizeof(SDL_Color));

    if (!palette->colors)
        goto fail;

    palette->ncolors = 256;

    format->BitsPerPixel = 8;
    format->BytesPerPixel = 1;
    format->palette = palette;
    format->Rmask = Rmask;
    format->Gmask = Gmask;
    format->Bmask = Bmask;
    format->Amask = Amask;

    surface->flags = flags;
    surface->format = format;
    surface->w = width;
    surface->h = height;
    surface->pitch = (Uint16)width;
    surface->pixels = calloc((size_t)width, (size_t)height);

    if (!surface->pixels)
        goto fail;

    return surface;

fail:
    if (palette) {
        free(palette->colors);
        free(palette);
    }
    free(format);
    free(surface);
    return NULL;
}

void SDL_FreeSurface(SDL_Surface *surface)
{
    if (!surface)
        return;

    if (surface->format) {
        if (surface->format->palette) {
            free(surface->format->palette->colors);
            free(surface->format->palette);
        }

        free(surface->format);
    }

    free(surface->pixels);
    free(surface);
}

int SDL_SetColors(SDL_Surface *surface, SDL_Color *colors,
                  int firstcolor, int ncolors)
{
    if (!surface || !surface->format ||
        !surface->format->palette || !colors ||
        firstcolor < 0 || ncolors < 0 ||
        firstcolor > 256 || ncolors > 256 - firstcolor)
        return 0;

    memcpy(
        &surface->format->palette->colors[firstcolor],
        colors,
        (size_t)ncolors * sizeof(SDL_Color)
    );

    return 1;
}

int SDL_SetPalette(SDL_Surface *surface, int flags,
                   SDL_Color *colors, int firstcolor, int ncolors)
{
    (void)flags;

    int result = SDL_SetColors(
        surface, colors, firstcolor, ncolors
    );

    // The Switch 2 framebuffer does not automatically
    // redraw when an 8-bit palette changes.
    // Refresh only the physical screen surface.
    if (result && surface == physicalScreen)
        SDL_Flip(surface);

    return result;
}

int SDL_LockSurface(SDL_Surface *surface)
{
    return surface ? 0 : -1;
}

void SDL_UnlockSurface(SDL_Surface *surface)
{
    (void)surface;
}

void wolfSetPhysicalScreen(SDL_Surface *surface)
{
    physicalScreen = surface;
}
