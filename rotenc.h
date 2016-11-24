#ifndef __BUTTON_HANDLER_H__
#define __BUTTON_HANDLER_H__

#include <Arduino.h>

#define BUTTON_STATE_UP     0x01
#define BUTTON_STATE_DOWN   0x00

#define TURN_STATE_NONE     0
#define TURN_STATE_CW       1
#define TURN_STATE_CCW      2

extern void setupRotEnc();
extern void updateRotEnc();

extern uint8_t button_state;
extern uint8_t turn_state;

#endif // __BUTTON_HANDLER_H__
