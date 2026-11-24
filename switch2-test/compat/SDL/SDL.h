#ifndef WOLF_SWITCH2_SDL_H
#define WOLF_SWITCH2_SDL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t Uint8;
typedef uint16_t Uint16;
typedef uint32_t Uint32;
typedef int16_t Sint16;

typedef struct SDL_Color {
    Uint8 r, g, b, unused;
} SDL_Color;

typedef struct SDL_Palette {
    int ncolors;
    SDL_Color *colors;
} SDL_Palette;

typedef struct SDL_PixelFormat {
    SDL_Palette *palette;
    Uint8 BitsPerPixel;
    Uint8 BytesPerPixel;
    Uint32 Rmask, Gmask, Bmask, Amask;
} SDL_PixelFormat;

typedef struct SDL_Rect {
    int16_t x, y;
    uint16_t w, h;
} SDL_Rect;

typedef struct SDL_Surface {
    Uint32 flags;
    SDL_PixelFormat *format;
    int w, h;
    Uint16 pitch;
    void *pixels;
} SDL_Surface;

#define SDL_SWSURFACE  0x00000000
#define SDL_HWSURFACE  0x00000001
#define SDL_DOUBLEBUF  0x40000000
#define SDL_HWPALETTE  0x20000000
#define SDL_FULLSCREEN 0x80000000

#define SDL_LOGPAL 0x01
#define SDL_PHYSPAL 0x02

#define SDL_MUSTLOCK(surface) 0

SDL_Surface *SDL_CreateRGBSurface(
    Uint32 flags, int width, int height, int depth,
    Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask
);

void SDL_FreeSurface(SDL_Surface *surface);
int SDL_Flip(SDL_Surface *screen);
SDL_Surface *SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags);

int SDL_BlitSurface(
    SDL_Surface *src, SDL_Rect *srcrect,
    SDL_Surface *dst, SDL_Rect *dstrect
);

int SDL_FillRect(
    SDL_Surface *dst, SDL_Rect *dstrect, Uint32 color
);

int SDL_SetColors(
    SDL_Surface *surface, SDL_Color *colors,
    int firstcolor, int ncolors
);

int SDL_SetPalette(
    SDL_Surface *surface, int flags,
    SDL_Color *colors, int firstcolor, int ncolors
);

int SDL_LockSurface(SDL_Surface *surface);
void SDL_UnlockSurface(SDL_Surface *surface);

Uint32 SDL_MapRGB(
    SDL_PixelFormat *format, Uint8 r, Uint8 g, Uint8 b
);

#ifdef __cplusplus
}
#endif


/* WOLF_SDL_CPP_LINKAGE_FIX */
#ifdef __cplusplus
extern "C" {
#endif

/* Wolf4SDL Switch 2 compatibility additions */

#ifndef WOLF_SDL_EXTRA
#define WOLF_SDL_EXTRA

typedef int SDLKey;
typedef struct SDL_Joystick SDL_Joystick;

#ifndef SDLK_LAST
#define SDLK_LAST 512
#endif

#define SDLK_a 97
#define SDLK_b 98
#define SDLK_q 113
#define SDLK_x 120
#define SDLK_y 121

#define SDLK_BACKSPACE 8
#define SDLK_ESCAPE 27
#define SDLK_SPACE 32
#define SDLK_TAB 9
#define SDLK_PLUS 43
#define SDLK_MINUS 45
#define SDLK_DELETE 127

#define SDLK_UP 273
#define SDLK_DOWN 274
#define SDLK_RIGHT 275
#define SDLK_LEFT 276
#define SDLK_INSERT 277
#define SDLK_HOME 278
#define SDLK_END 279
#define SDLK_PAGEUP 280
#define SDLK_PAGEDOWN 281

#define SDLK_F1 282
#define SDLK_F2 283
#define SDLK_F3 284
#define SDLK_F4 285
#define SDLK_F5 286
#define SDLK_F6 287
#define SDLK_F7 288
#define SDLK_F8 289
#define SDLK_F9 290
#define SDLK_F10 291

#define SDLK_KP5 261

unsigned int SDL_GetTicks(void);
void SDL_Delay(unsigned int milliseconds);
const char *SDL_GetError(void);

#endif


/* WOLF_SDL_VIDEO_INFO */
typedef struct SDL_VideoInfo {
    Uint32 hw_available;
    Uint32 wm_available;
    SDL_PixelFormat *vfmt;
    int current_w;
    int current_h;
} SDL_VideoInfo;

const SDL_VideoInfo *SDL_GetVideoInfo(void);

/* Switch 2 SDL event compatibility */

#ifndef WOLF_SDL_EVENTS
#define WOLF_SDL_EVENTS

typedef int SDLMod;

#define KMOD_NONE  0x0000
#define KMOD_SHIFT 0x0003
#define KMOD_NUM   0x1000
#define KMOD_CAPS  0x2000

#define SDLK_RETURN     13
#define SDLK_PAUSE      19
#define SDLK_SCROLLOCK  302
#define SDLK_RSHIFT     303
#define SDLK_LSHIFT     304
#define SDLK_RCTRL      305
#define SDLK_LCTRL      306
#define SDLK_RALT       307
#define SDLK_LALT       308
#define SDLK_F12        293

#define SDLK_KP2        258
#define SDLK_KP4        260
#define SDLK_KP6        262
#define SDLK_KP8        264
#define SDLK_KP_ENTER   271

#define SDL_ACTIVEEVENT 1
#define SDL_KEYDOWN     2
#define SDL_KEYUP       3
#define SDL_MOUSEMOTION 4
#define SDL_QUIT        12

#define SDL_APPACTIVE   0x04
#define SDL_IGNORE     0
#define SDL_DISABLE    0
#define SDL_ENABLE     1

#define SDL_GRAB_OFF    0
#define SDL_GRAB_ON     1

#define SDL_BUTTON(X) (1 << ((X)-1))
#define SDL_BUTTON_LEFT   1
#define SDL_BUTTON_MIDDLE 2
#define SDL_BUTTON_RIGHT  3

#define SDL_HAT_UP    0x01
#define SDL_HAT_RIGHT 0x02
#define SDL_HAT_DOWN  0x04
#define SDL_HAT_LEFT  0x08

typedef struct SDL_keysym {
    unsigned char scancode;
    SDLKey sym;
    SDLMod mod;
    unsigned short unicode;
} SDL_keysym;

typedef struct SDL_KeyboardEvent {
    unsigned char type;
    unsigned char which;
    unsigned char state;
    SDL_keysym keysym;
} SDL_KeyboardEvent;

typedef struct SDL_ActiveEvent {
    unsigned char type;
    unsigned char gain;
    unsigned char state;
} SDL_ActiveEvent;

typedef union SDL_Event {
    unsigned char type;
    SDL_KeyboardEvent key;
    SDL_ActiveEvent active;
} SDL_Event;

SDLMod SDL_GetModState(void);
int SDL_PollEvent(SDL_Event *event);
int SDL_WaitEvent(SDL_Event *event);
unsigned char SDL_EventState(unsigned char type, int state);

unsigned char SDL_GetMouseState(int *x, int *y);
void SDL_WarpMouse(unsigned short x, unsigned short y);
int SDL_ShowCursor(int toggle);
int SDL_WM_GrabInput(int mode);
void SDL_WM_SetCaption(const char *title, const char *icon);

int SDL_NumJoysticks(void);
SDL_Joystick *SDL_JoystickOpen(int index);
void SDL_JoystickClose(SDL_Joystick *joystick);
void SDL_JoystickUpdate(void);
int SDL_JoystickNumButtons(SDL_Joystick *joystick);
int SDL_JoystickNumHats(SDL_Joystick *joystick);
short SDL_JoystickGetAxis(SDL_Joystick *joystick, int axis);
unsigned char SDL_JoystickGetHat(SDL_Joystick *joystick, int hat);
unsigned char SDL_JoystickGetButton(SDL_Joystick *joystick, int button);

#endif

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
extern "C" {
#endif

int SDL_SaveBMP(SDL_Surface *surface, const char *filename);

#ifdef __cplusplus
}
#endif


/* SDL subsystem initialization flags */
#define SDL_INIT_VIDEO     0x00000020
#define SDL_INIT_AUDIO     0x00000010
#define SDL_INIT_JOYSTICK  0x00000200

#ifdef __cplusplus
extern "C" {
#endif

int SDL_Init(Uint32 flags);
void SDL_Quit(void);

#ifdef __cplusplus
}
#endif

#endif
