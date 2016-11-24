#include "rotenc.h"

#define DEBOUNCE_DELAY    50
#define CHANGE_DELAY      50

#define ROTENC_CLK      12
#define ROTENC_DATA     11
#define ROTENC_BUTTON   10

#define ENC_STT_IDLE    0x00
#define ENC_STT_CW      0x01
#define ENC_STT_CCW     0x02

#define PULSES_PER_STEP   4

uint8_t button_state = (BUTTON_STATE_UP << 1) | BUTTON_STATE_UP;
uint8_t button

    static uint8_t _readEncoder();

    static uint8_t _buttonState[];
    static uint8_t _lastButtonState[];
    static bool _hasChanged[];
    static unsigned long _lastDebounceTime[];
    static unsigned long _lastChangeTime[];

    static uint8_t _turn;
    static uint8_t _cwRotorState[];
    static uint8_t _ccwRotorState[];

uint8_t ButtonHandler::_buttonState[NUM_BUTTONS] = {BUTTON_STATE_UP, BUTTON_STATE_UP, BUTTON_STATE_UP, BUTTON_STATE_UP, BUTTON_STATE_UP, BUTTON_STATE_UP, BUTTON_STATE_UP, BUTTON_STATE_UP};
uint8_t ButtonHandler::_lastButtonState[NUM_BUTTONS] = {BUTTON_STATE_UP, BUTTON_STATE_UP, BUTTON_STATE_UP, BUTTON_STATE_UP, BUTTON_STATE_UP, BUTTON_STATE_UP, BUTTON_STATE_UP, BUTTON_STATE_UP};
bool ButtonHandler::_hasChanged[NUM_BUTTONS] = {false, false, false, false, false, false, false, false};
unsigned long ButtonHandler::_lastDebounceTime[NUM_BUTTONS] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned long ButtonHandler::_lastChangeTime[NUM_BUTTONS] = {millis(), millis(), millis(), millis(), millis(), millis(), millis(), millis()};
uint8_t ButtonHandler::_turn = TURN_NONE;

uint8_t ButtonHandler::_cwRotorState[4] = {B10, B00, B11, B01};
uint8_t ButtonHandler::_ccwRotorState[4] = {B01, B11, B00, B10};

uint8_t ButtonHandler::_readEncoder() {
    static uint8_t prevRotorState = 0;
    static int bufferedCounts = 0;

    uint8_t result = 0;

    uint8_t startState = digitalRead(ROTENC_DATA) | (digitalRead(ROTENC_CLK) << 1) | (digitalRead(ROTENC_BUTTON) << 2); // Get current state
    delayMicroseconds(DEBOUNCE_DELAY); // Wait safety bounce time
    uint8_t stopState = digitalRead(ROTENC_DATA) | (digitalRead(ROTENC_CLK) << 1) | (digitalRead(ROTENC_BUTTON) << 2); // Get current state
  
    if (startState == stopState) { // Check if the state was stable
      // Interpret rotor state
      uint8_t rotorState = stopState & B00000011; // That's the A and B pin state
      if (rotorState != prevRotorState) { // Check if rotor state has changed
          if (rotorState == ButtonHandler::_cwRotorState[prevRotorState]) {
            bufferedCounts++; 
          } else if (rotorState == ButtonHandler::_ccwRotorState[prevRotorState]) {
            bufferedCounts--; 
          } else {
            bufferedCounts = 0;   
          }
        
          if (abs(bufferedCounts) == PULSES_PER_STEP) {
            if (bufferedCounts > 0) {
                result |= ENC_STT_CCW;
            }
            if (bufferedCounts < 0) {
                result |= ENC_STT_CW;  
            }
            bufferedCounts = 0;
          }
      
          prevRotorState = rotorState; // Record state for next pulse interpretation
      }
    }

    return result;
}

void ButtonHandler::setup() {
  debugPrintf(F("Initialize Buttons"));
 
  pinMode(PIN_BTN_PLOAD, OUTPUT);
  pinMode(PIN_BTN_CLKEN, OUTPUT);
  pinMode(PIN_BTN_CLK, OUTPUT);
  pinMode(PIN_BTN_DATAIN, INPUT);

  digitalWrite(PIN_BTN_CLK, LOW);
  digitalWrite(PIN_BTN_PLOAD, HIGH);

  pinMode(ROTENC_DATA, INPUT);
  pinMode(ROTENC_CLK, INPUT);
  pinMode(ROTENC_BUTTON, INPUT);
  digitalWrite(ROTENC_CLK, HIGH);
  digitalWrite(ROTENC_DATA, HIGH);
  digitalWrite(ROTENC_BUTTON, HIGH);
}

void ButtonHandler::update() {
  // Trigger a parallel load to latch the state of the data lines
  digitalWrite(PIN_BTN_CLKEN, HIGH);
  digitalWrite(PIN_BTN_PLOAD, LOW);
  delayMicroseconds(PULSE_WIDTH_USEC);
  digitalWrite(PIN_BTN_PLOAD, HIGH);
  digitalWrite(PIN_BTN_CLKEN, LOW);

  // Loop to read each bit value from the serial out line of the SN74HC165N.
  for (uint8_t i = 0; i < DATA_WIDTH; i++) {
    // Menu button will be handled seperate
    if (i < NUM_BUTTONS && i > 0) {
      uint8_t state = digitalRead(PIN_BTN_DATAIN);
      uint8_t button = NUM_BUTTONS - i - 1;

      if (state != ButtonHandler::_lastButtonState[button]) {
        ButtonHandler::_lastDebounceTime[button] = millis();
      } else if ((millis() - ButtonHandler::_lastDebounceTime[button]) > DEBOUNCE_DELAY) {
        if (state != ButtonHandler::_buttonState[button]) {
          ButtonHandler::_buttonState[button] = state;
          ButtonHandler::_lastChangeTime[button] = millis();
          ButtonHandler::_hasChanged[button] = true;
        } else {
          ButtonHandler::_hasChanged[button] = false;
        }
      }

      ButtonHandler::_lastButtonState[button] = state;

      #ifdef DEBUG
        if (ButtonHandler::isReleased(button)) {
          debugPrintf(F("Button %d released"), button);
        }

        if (ButtonHandler::isPressed(button)) {
          debugPrintf(F("Button %d pressed"), button);
        }
      #endif
    }
  
    // Pulse the Clock (rising edge shifts the next bit).
    digitalWrite(PIN_BTN_CLK, HIGH);
    delayMicroseconds(PULSE_WIDTH_USEC);
    digitalWrite(PIN_BTN_CLK, LOW);
  }

  // Handle menu button
  uint8_t state = digitalRead(ROTENC_BUTTON);
  if (state != ButtonHandler::_lastButtonState[BUTTON_MENU]) {
    ButtonHandler::_lastDebounceTime[BUTTON_MENU] = millis();
  } else if ((millis() - ButtonHandler::_lastDebounceTime[BUTTON_MENU]) > DEBOUNCE_DELAY) {
    if (state != ButtonHandler::_buttonState[BUTTON_MENU]) {
      ButtonHandler::_buttonState[BUTTON_MENU] = state;
      ButtonHandler::_lastChangeTime[BUTTON_MENU] = millis();
      ButtonHandler::_hasChanged[BUTTON_MENU] = true;
    } else {
      ButtonHandler::_hasChanged[BUTTON_MENU] = false;
    }
  }

  ButtonHandler::_lastButtonState[BUTTON_MENU] = state;

  #ifdef DEBUG
    if (ButtonHandler::isReleased(BUTTON_MENU)) {
      debugPrintf(F("Button %d released"), BUTTON_MENU);
    }

    if (ButtonHandler::isPressed(BUTTON_MENU)) {
      debugPrintf(F("Button %d pressed"), BUTTON_MENU);
    }
  #endif

  ButtonHandler::_turn = TURN_NONE;
    uint8_t encoderRead = ButtonHandler::_readEncoder();
    if (encoderRead) {
      if (encoderRead & ENC_STT_CW) {
        ButtonHandler::_turn = TURN_UP;
      } else if (encoderRead & ENC_STT_CCW) {
        ButtonHandler::_turn = TURN_DOWN;
      }
    }
}

bool ButtonHandler::isTurnedUp() {
  return ButtonHandler::_turn == TURN_UP;
}

bool ButtonHandler::isTurnedDown() {
  return ButtonHandler::_turn == TURN_DOWN;
}

bool ButtonHandler::isUp(uint8_t button) {
  return ButtonHandler::_buttonState[button] == BUTTON_STATE_UP;
}

bool ButtonHandler::isUp(uint8_t button, unsigned long forTime) {
  
}

bool ButtonHandler::isDown(uint8_t button) {
  return ButtonHandler::_buttonState[button] == BUTTON_STATE_DOWN;
}

bool ButtonHandler::isPressed(uint8_t button) {
  return ButtonHandler::_hasChanged[button] && ButtonHandler::_buttonState[button] == BUTTON_STATE_DOWN;
}

bool ButtonHandler::isReleased(uint8_t button) {
  return ButtonHandler::_hasChanged[button] && ButtonHandler::_buttonState[button] == BUTTON_STATE_UP;
}
