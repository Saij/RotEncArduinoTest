#ifndef __BUTTON_HANDLER_H__
#define __BUTTON_HANDLER_H__

#include <Arduino.h>

#define BUTTON_STATE_UP     0x01
#define BUTTON_STATE_DOWN   0x00

#define TURN_STATE_NONE     0
#define TURN_STATE_CW       1
#define TURN_STATE_CCW      2

void setupRotEnc();
void updateRotEnc();
uint8_t getButtonState();
uint8_t getTurnState();

#endif // __BUTTON_HANDLER_H__
