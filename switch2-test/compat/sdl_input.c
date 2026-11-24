#include <SDL/SDL.h>
#include "wolf_switch2_input.h"
#include <stddef.h>

struct SDL_Joystick {
    int index;
};

static struct SDL_Joystick controller = {0};

static const u64 buttonMap[] = {
    KEY_A,
    KEY_X,
    KEY_B,
    KEY_Y,
    KEY_L,
    KEY_R,
    KEY_MINUS,
    KEY_PLUS,
    KEY_ZL,
    KEY_ZR
};

int SDL_NumJoysticks(void)
{
    wolfSwitch2InputInit();
    return 1;
}

SDL_Joystick *SDL_JoystickOpen(int index)
{
    if (index != 0)
        return NULL;

    wolfSwitch2InputInit();
    return &controller;
}

void SDL_JoystickClose(SDL_Joystick *joystick)
{
    (void)joystick;
}

void SDL_JoystickUpdate(void)
{
    wolfSwitch2InputUpdate();
}

int SDL_JoystickNumButtons(SDL_Joystick *joystick)
{
    return joystick ? 10 : 0;
}

int SDL_JoystickNumHats(SDL_Joystick *joystick)
{
    return joystick ? 1 : 0;
}

short SDL_JoystickGetAxis(SDL_Joystick *joystick, int axis)
{
    JoystickPosition pos = {0, 0};

    if (!joystick || axis < 0 || axis > 3)
        return 0;

    wolfSwitch2JoystickRead(
        &pos,
        CONTROLLER_P1_AUTO,
        axis >= 2 ? JOYSTICK_RIGHT : JOYSTICK_LEFT
    );

    int value = (axis % 2 == 0) ? pos.dx : -pos.dy;

    if (value > 32767)
        value = 32767;
    if (value < -32768)
        value = -32768;

    return (short)value;
}

unsigned char SDL_JoystickGetHat(SDL_Joystick *joystick, int hat)
{
    if (!joystick || hat != 0)
        return 0;

    u64 keys = wolfSwitch2KeysHeld();
    unsigned char result = 0;

    if (keys & KEY_DUP)
        result |= SDL_HAT_UP;
    if (keys & KEY_DDOWN)
        result |= SDL_HAT_DOWN;
    if (keys & KEY_DLEFT)
        result |= SDL_HAT_LEFT;
    if (keys & KEY_DRIGHT)
        result |= SDL_HAT_RIGHT;

    return result;
}

unsigned char SDL_JoystickGetButton(SDL_Joystick *joystick, int button)
{
    if (!joystick || button < 0 ||
        button >= (int)(sizeof(buttonMap) / sizeof(buttonMap[0])))
        return 0;

    return (wolfSwitch2KeysHeld() & buttonMap[button]) ? 1 : 0;
}
