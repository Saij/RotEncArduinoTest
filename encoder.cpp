#include "encoder.h"


#define PHASE_A (ENC_PIN & 1 << ENC_PIN_A)
#define PHASE_B (ENC_PIN & 1 << ENC_PIN_B)

#define sbi(port, bit) (port |= (1 << bit))
#define cbi(port, bit) (port &= ~(1 << bit))

unsigned char enc_buffer = 0; // Buffer for readings

void encInit(void) {
  ENC_DDR &= ~((1 << ENC_PIN_A) | (1 << ENC_PIN_B) | (1 << ENC_BTN));
  ENC_PORT |= (1 << ENC_PIN_A) | (1 << ENC_PIN_B) | (1 << ENC_BTN);
}


void encPoll(void) {
  static unsigned char enc_state;
  unsigned char tmp;  
  unsigned char cur_state = 0;

  if (PHASE_A) {
    sbi(cur_state, 0);
  }
  if (PHASE_B) {
    sbi(cur_state, 1);
  }

  tmp = enc_state;
  if (cur_state == (tmp & 0x03)) {
    return;
  }

  tmp = (tmp << 2) | cur_state;
  enc_state = tmp;

  if (tmp == 0xE1) {
    enc_buffer = SPIN_CCW;
  }
  if (tmp == 0xD2) {
    enc_buffer = SPIN_CW;
  }
}

unsigned char encGetState(void) {
  unsigned char tmp = enc_buffer;
  enc_buffer = 0;
  return tmp;
}

unsigned char encGetButtonState(void) {
  return (ENC_PIN & (1 << ENC_BTN)) ? 0 : 1;
}


