#include <stdio.h>
#include <SDL/SDL.h>
#include "wolf_switch2_input.h"
#include <string.h>

typedef struct {
    u64 button;
    SDLKey key;
} KeyMapping;

static const KeyMapping mappings[] = {
    {KEY_DUP,    SDLK_UP},
    {KEY_DDOWN,  SDLK_DOWN},
    {KEY_DLEFT,  SDLK_LEFT},
    {KEY_DRIGHT, SDLK_RIGHT},

    // Menu controls
    {KEY_A,      SDLK_RETURN},
    {KEY_B,      SDLK_ESCAPE},

    // Gameplay controls
    {KEY_Y,      SDLK_a},
    {KEY_ZR,     SDLK_a},
    {KEY_ZL,     SDLK_b},
    {KEY_X,      SDLK_y},

    {KEY_PLUS,   SDLK_RETURN},
    {KEY_MINUS,  SDLK_ESCAPE}
};

static u64 previousKeys = 0;
static SDL_Event queue[32];
static int queueCount = 0;

static void pushKey(SDLKey key, int pressed)
{
    if(queueCount >= 32)
        return;

    SDL_Event *event = &queue[queueCount++];
    memset(event, 0, sizeof(*event));

    event->type = pressed ? SDL_KEYDOWN : SDL_KEYUP;
    event->key.state = pressed ? 1 : 0;
    event->key.keysym.sym = key;

    FILE *log = fopen("/switch/wolf4sdl/event-debug.txt", "a");
    if(log)
    {
        fprintf(log, "key=%d pressed=%d\\n", (int)key, pressed);
        fclose(log);
    }
}

static void updateEvents(void)
{
    wolfSwitch2InputUpdate();

    u64 keys = wolfSwitch2KeysHeld();

    static u64 lastLoggedKeys = ~(u64)0;
    if (keys != lastLoggedKeys)
    {
        FILE *log = fopen("/switch/wolf4sdl/input-debug.txt", "a");
        if (log)
        {
            fprintf(log, "buttons=0x%016llx\\n",
                    (unsigned long long)keys);
            fclose(log);
        }
        lastLoggedKeys = keys;
    }

    for(unsigned i = 0;
        i < sizeof(mappings) / sizeof(mappings[0]);
        i++)
    {
        u64 mask = mappings[i].button;

        if((keys & mask) != (previousKeys & mask))
            pushKey(mappings[i].key, (keys & mask) != 0);
    }

    previousKeys = keys;
}

int SDL_PollEvent(SDL_Event *event)
{
    updateEvents();

    if(queueCount == 0)
        return 0;

    if(event)
        *event = queue[0];

    memmove(queue, queue + 1,
            sizeof(SDL_Event) * (queueCount - 1));

    queueCount--;
    return 1;
}

int SDL_WaitEvent(SDL_Event *event)
{
    while(!SDL_PollEvent(event))
        SDL_Delay(1);

    return 1;
}

SDLMod SDL_GetModState(void)
{
    return KMOD_NONE;
}

unsigned char SDL_EventState(unsigned char type, int state)
{
    (void)type;
    (void)state;
    return SDL_ENABLE;
}

unsigned char SDL_GetMouseState(int *x, int *y)
{
    if(x) *x = 0;
    if(y) *y = 0;
    return 0;
}

void SDL_WarpMouse(unsigned short x, unsigned short y)
{
    (void)x;
    (void)y;
}

int SDL_ShowCursor(int toggle)
{
    (void)toggle;
    return 0;
}

int SDL_WM_GrabInput(int mode)
{
    return mode;
}

void SDL_WM_SetCaption(const char *title, const char *icon)
{
    (void)title;
    (void)icon;
}
