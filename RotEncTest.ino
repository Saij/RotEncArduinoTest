#include "debug.h"
#include "encoder.h"

void setup() {
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);
  
  setupDebug();
  encInit();
}

void loop() {
  encPoll();
  unsigned char encstate = encGetState();
  unsigned char btn = encGetButtonState();

  if (SPIN_CCW == encstate) {
    debugPrintf(F("SPIN_CCW"));
    digitalWrite(8, HIGH);
    delay(10);
    digitalWrite(8, LOW);
  } else if (SPIN_CW == encstate) {
    debugPrintf(F("SPIN_CW"));
    digitalWrite(8, HIGH);
    delay(10);
    digitalWrite(8, LOW);
  } else if (btn) {
    debugPrintf(F("BTN"));
    digitalWrite(8, HIGH);
    delay(10);
    digitalWrite(8, LOW);
  }
}
