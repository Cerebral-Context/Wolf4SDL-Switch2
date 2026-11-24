#include "wolf_switch2_input.h"
#include <string.h>

static PadState wolfPad;
static int initialized = 0;

void wolfSwitch2InputInit(void)
{
    if (initialized)
        return;

    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&wolfPad);
    initialized = 1;
}

void wolfSwitch2InputUpdate(void)
{
    wolfSwitch2InputInit();
    padUpdate(&wolfPad);
}

u64 wolfSwitch2KeysHeld(void)
{
    wolfSwitch2InputInit();
    return padGetButtons(&wolfPad);
}

void wolfSwitch2JoystickRead(
    JoystickPosition *position,
    int controller,
    int stick)
{
    (void)controller;

    if (!position)
        return;

    wolfSwitch2InputInit();

    HidAnalogStickState analog =
        padGetStickPos(&wolfPad, stick == JOYSTICK_RIGHT ? 1 : 0);

    position->dx = analog.x;
    position->dy = analog.y;
}
