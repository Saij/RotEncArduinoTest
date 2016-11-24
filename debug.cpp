#include "debug.h"
#include <Arduino.h>

#ifdef DEBUG

void setupDebug() {
  Serial.begin(38400);
  Serial.println(F("RotEnc - Debug"));
  Serial.println(F("--------------"));
}

void debugPrintf(const char *format, ...) {
  char buf[PRINTF_BUF];
  va_list ap;
  va_start(ap, format);
  vsnprintf(buf, sizeof(buf), format, ap);
  for (char *p = &buf[0]; *p; p++) { // emulate cooked mode for newlines
    if (*p == '\n') {
      Serial.write('\r');
    }
    Serial.write(*p);
  }
  Serial.write("\r\n");
  va_end(ap);
}

#ifdef F // check to see if F() macro is available
void debugPrintf(const __FlashStringHelper *format, ...) {
  char buf[PRINTF_BUF];
  va_list ap;
  va_start(ap, format);

  #ifdef __AVR__
    vsnprintf_P(buf, sizeof(buf), (const char *)format, ap); // progmem for AVR
  #else
    vsnprintf(buf, sizeof(buf), (const char *)format, ap); // for the rest of the world
  #endif

  for (char *p = &buf[0]; *p; p++) { // emulate cooked mode for newlines
    if (*p == '\n') {
      Serial.write('\r');
    }
    Serial.write(*p);
  }
  Serial.write("\r\n");
  va_end(ap);
}
#endif // F

#endif // DEBUG
