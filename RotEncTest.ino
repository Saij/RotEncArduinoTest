#include "debug.h"
#include "encoder.h"
#include "settings.h"

void setup() {
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);
  
  setupDebug();
  settingsInit();
  encInit();
}

void loop() {
  encPoll();
  unsigned char encstate = encGetState();
  unsigned char btn = encGetButtonState();

  if (SPIN_CCW == encstate) {
    debugPrintf(F("SPIN_CCW 0x%02X%02X"), settingsGetModifiers(REGISTER_CCW), settingsGetKeycode(REGISTER_CCW));
    digitalWrite(8, HIGH);
    delay(10);
    digitalWrite(8, LOW);
  } else if (SPIN_CW == encstate) {
    debugPrintf(F("SPIN_CW 0x%02X%02X"), settingsGetModifiers(REGISTER_CW), settingsGetKeycode(REGISTER_CW));
    digitalWrite(8, HIGH);
    delay(10);
    digitalWrite(8, LOW);
  } else if (btn) {
    debugPrintf(F("BTN 0x%02X%02X"), settingsGetModifiers(REGISTER_BTN), settingsGetKeycode(REGISTER_BTN));
    digitalWrite(8, HIGH);
    delay(10);
    digitalWrite(8, LOW);
  }
}
