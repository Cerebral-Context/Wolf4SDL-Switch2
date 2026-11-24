#include <SDL/SDL.h>
#include <stdint.h>
#include <string.h>

int SDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect,
                    SDL_Surface *dst, SDL_Rect *dstrect)
{
    if (!src || !dst || !src->pixels || !dst->pixels)
        return -1;

    if (!src->format || !dst->format ||
        src->format->BytesPerPixel != 1 ||
        dst->format->BytesPerPixel != 1)
        return -1;

    int sx = srcrect ? srcrect->x : 0;
    int sy = srcrect ? srcrect->y : 0;
    int dx = dstrect ? dstrect->x : 0;
    int dy = dstrect ? dstrect->y : 0;

    int w = srcrect ? srcrect->w : src->w;
    int h = srcrect ? srcrect->h : src->h;

    // Clip to the source and destination boundaries.
    if (sx < 0) { dx -= sx; w += sx; sx = 0; }
    if (sy < 0) { dy -= sy; h += sy; sy = 0; }
    if (dx < 0) { sx -= dx; w += dx; dx = 0; }
    if (dy < 0) { sy -= dy; h += dy; dy = 0; }

    if (sx + w > src->w) w = src->w - sx;
    if (sy + h > src->h) h = src->h - sy;
    if (dx + w > dst->w) w = dst->w - dx;
    if (dy + h > dst->h) h = dst->h - dy;

    if (w <= 0 || h <= 0)
        return 0;

    for (int y = 0; y < h; y++) {
        uint8_t *s = (uint8_t *)src->pixels +
                     (sy + y) * src->pitch + sx;

        uint8_t *d = (uint8_t *)dst->pixels +
                     (dy + y) * dst->pitch + dx;

        memmove(d, s, (size_t)w);
    }

    return 0;
}

int SDL_FillRect(SDL_Surface *dst, SDL_Rect *rect, Uint32 color)
{
    if (!dst || !dst->pixels || !dst->format ||
        dst->format->BytesPerPixel != 1)
        return -1;

    int x = rect ? rect->x : 0;
    int y = rect ? rect->y : 0;
    int w = rect ? rect->w : dst->w;
    int h = rect ? rect->h : dst->h;

    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }

    if (x + w > dst->w) w = dst->w - x;
    if (y + h > dst->h) h = dst->h - y;

    if (w <= 0 || h <= 0)
        return 0;

    for (int row = 0; row < h; row++) {
        uint8_t *pixels = (uint8_t *)dst->pixels +
                          (y + row) * dst->pitch + x;

        memset(pixels, (uint8_t)color, (size_t)w);
    }

    return 0;
}
