#ifndef WOLF_SWITCH2_INPUT_H
#define WOLF_SWITCH2_INPUT_H

#include <switch.h>

typedef struct {
    int dx;
    int dy;
} JoystickPosition;

#define CONTROLLER_P1_AUTO 0
#define JOYSTICK_LEFT  0
#define JOYSTICK_RIGHT 1

#define KEY_A      HidNpadButton_A
#define KEY_B      HidNpadButton_B
#define KEY_X      HidNpadButton_X
#define KEY_Y      HidNpadButton_Y
#define KEY_L      HidNpadButton_L
#define KEY_R      HidNpadButton_R
#define KEY_ZL     HidNpadButton_ZL
#define KEY_ZR     HidNpadButton_ZR
#define KEY_PLUS   HidNpadButton_Plus
#define KEY_MINUS  HidNpadButton_Minus

#define KEY_DUP    HidNpadButton_Up
#define KEY_DDOWN  HidNpadButton_Down
#define KEY_DLEFT  HidNpadButton_Left
#define KEY_DRIGHT HidNpadButton_Right

#ifdef __cplusplus
extern "C" {
#endif

void wolfSwitch2InputInit(void);
void wolfSwitch2InputUpdate(void);
u64 wolfSwitch2KeysHeld(void);
void wolfSwitch2JoystickRead(
    JoystickPosition *position, int controller, int stick
);

#ifdef __cplusplus
}
#endif

#define hidScanInput() wolfSwitch2InputUpdate()
#define hidKeysHeld(controller) wolfSwitch2KeysHeld()
#define hidJoystickRead(position,controller,stick) \
    wolfSwitch2JoystickRead(position,controller,stick)

#endif
