#include "encoder.h"
#include "settings.h"

#include <util/delay.h>

int main() {
  DDRB |= (1 << PB0);
  PORTB &= ~(1 << PB0);
  
  settingsInit();
  encInit();

  while(1) {
    encPoll();
    unsigned char encstate = encGetState();
    unsigned char btn = encGetButtonState();

    if (SPIN_CCW == encstate) {
      PORTB |= (1 << PB0);
      _delay_ms(10);
      PORTB &= ~(1 << PB0);
    } else if (SPIN_CW == encstate) {
      PORTB |= (1 << PB0);
      _delay_ms(10);
      PORTB &= ~(1 << PB0);
    } else if (btn) {
      PORTB |= (1 << PB0);
      _delay_ms(10);
      PORTB &= ~(1 << PB0);
    }
  }

  return 0;
}