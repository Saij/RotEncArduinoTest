#include "debug.h"
#include "rotenc.h"

void setup() {
  setupDebug();
  setupRotEnc();
}

void loop() {
  updateRotEnc();

  if (button_state != 0 || turn_state != 0)
    debugPrintf(F("Button State: %d\tTurn State: %d"), button_state, turn_state);
}
