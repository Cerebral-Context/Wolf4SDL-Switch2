#include <SDL/SDL.h>

const SDL_VideoInfo *SDL_GetVideoInfo(void)
{
    static SDL_PixelFormat format = {
        .BitsPerPixel = 8,
        .BytesPerPixel = 1
    };

    static SDL_VideoInfo info = {
        .vfmt = &format,
        .current_w = 320,
        .current_h = 200
    };

    return &info;
}
