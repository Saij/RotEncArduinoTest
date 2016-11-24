#include "rotenc.h"

#include <util/delay.h>

#define DEBOUNCE_DELAY    50
#define CHANGE_DELAY      50

#define ROTENC_CLK      12
#define ROTENC_DATA     11
#define ROTENC_BUTTON   10

#define PULSES_PER_STEP   4

const uint8_t cw_rotor_state[4] = {B10, B00, B11, B01};
const uint8_t ccw_rotor_state[4] = {B01, B11, B00, B10};

uint8_t button_state = BUTTON_STATE_UP;
uint8_t turn_state = TURN_STATE_NONE;

void updateRotEnc() {
  static uint8_t prev_rotor_state = 0;
  static int buffered_counts = 0;

  turn_state = TURN_STATE_NONE;

  uint8_t start_state = digitalRead(ROTENC_DATA) | (digitalRead(ROTENC_CLK) << 1) | (digitalRead(ROTENC_BUTTON) << 2); // Get current state
  _delay_us(DEBOUNCE_DELAY); // Wait safety bounce time
  uint8_t stop_state = digitalRead(ROTENC_DATA) | (digitalRead(ROTENC_CLK) << 1) | (digitalRead(ROTENC_BUTTON) << 2); // Get current state

  // Check if the state was stable
  if (start_state == stop_state) { 
    // Interpret rotor state
    uint8_t rotor_state = stop_state & B00000011; // That's the A and B pin state
    if (rotor_state != prev_rotor_state) { // Check if rotor state has changed
      if (rotor_state == cw_rotor_state[prev_rotor_state]) {
        buffered_counts++; 
      } else if (rotor_state == ccw_rotor_state[prev_rotor_state]) {
        buffered_counts--; 
      } else {
        buffered_counts = 0;   
      }
      
      if (abs(buffered_counts) == PULSES_PER_STEP) {
        if (buffered_counts > 0) {
          turn_state = TURN_STATE_CCW;
        }
        if (buffered_counts < 0) {
            turn_state = TURN_STATE_CW;
        }
        buffered_counts = 0;
      }
  
      prev_rotor_state = rotor_state; // Record state for next pulse interpretation
    }
  }
}

void setupRotEnc() {
  // debugPrintf(F("Initialize Buttons"));
 
  pinMode(ROTENC_DATA, INPUT);
  pinMode(ROTENC_CLK, INPUT);
  pinMode(ROTENC_BUTTON, INPUT);
  digitalWrite(ROTENC_CLK, HIGH);
  digitalWrite(ROTENC_DATA, HIGH);
  digitalWrite(ROTENC_BUTTON, HIGH);
}
