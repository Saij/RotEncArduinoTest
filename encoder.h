#ifndef  __ENCODER_H__
#define  __ENCODER_H__

#include <avr/io.h>

/*
 * Ports and Pins
 */
#define ENC_PIN_A PB4
#define ENC_PIN_B PB3
#define ENC_BTN   PB2

#define ENC_PIN   PINB
#define ENC_DDR   DDRB
#define ENC_PORT  PORTB

/*
 * Spin direction
 */
#define SPIN_CW   0x01 
#define SPIN_CCW  0x10

void encInit(void);
void encPoll(void);
unsigned char encGetState(void);
unsigned char encGetButtonState(void);

#endif // __ENCODER_H__

