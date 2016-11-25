#include "settings.h"
#include "debug.h"
#include <EEPROMex.h>

#define NUM_REGISTERS 5

#define MAGIC_CODE  0x554B
#define VERSION     0x0001

#define MASK_KEY      0x00FF
#define MASK_MOD      0xFF00

uint8_t var_size          = ((NUM_REGISTERS << 1) | 1);
uint8_t buffer_len        = (EEPROMSizeATmega328 / var_size);
uint8_t addr_status_buffer = EEPROMSizeATmega328 - buffer_len;

uint16_t settings[NUM_REGISTERS] = {
  MAGIC_CODE,
  VERSION,
  0x0056, // CCW, no modifiers, Keypad -
  0x0057, // CW, no modifiers, Keypad +
  0x0067, // BTN, no modifiers, Keypad =
};

uint16_t _settingsFindNextWriteIndex() { 
  uint16_t i;
  for (i = addr_status_buffer; i < (buffer_len + addr_status_buffer); i++) {
      uint16_t prev_index;
    
      // Wrap around case
      if (i == addr_status_buffer) {
          prev_index = addr_status_buffer + buffer_len - 1;
      } else {
          prev_index = i - 1;
        }

      uint8_t prev_elem = EEPROM.readByte(prev_index);
      uint8_t curr_elem = EEPROM.readByte(i);

      // Must truncate the addition because the index tracking relies of wrap around
      if (((prev_elem + 1) & 0xFF) != curr_elem) {
          return i - addr_status_buffer;
      }
  }

  return 0;
}

void _settingsLoad() {
  uint16_t write_offset = _settingsFindNextWriteIndex() * var_size;
  uint16_t addr_read;

  addr_read = write_offset - var_size;
  if (write_offset == 0) {
    addr_read = addr_status_buffer - var_size;
  }

  debugPrintf(F("\tRead Address: %04X"), addr_read);

  uint16_t magic_code = EEPROM.readInt(addr_read);
  uint16_t version = EEPROM.readInt(addr_read + 0x02);

  debugPrintf(F("\tMagic Code: %04X\n\tVersion: %04X"), magic_code, version);

  if (magic_code == settings[REGISTER_MAGIC] && version == settings[REGISTER_VERSION]) {
    debugPrintf(F("\tLoading settings"));
    EEPROM.readBlock<uint16_t>(addr_read, settings, NUM_REGISTERS);
  }
}

void _settingsSave() {
  uint16_t write_offset = _settingsFindNextWriteIndex();
  debugPrintf(F("\tWrite Offset: %04X"), write_offset);

  EEPROM.updateBlock<uint16_t>(write_offset * var_size, settings, NUM_REGISTERS);

  // Update status buffer 
  uint16_t curr_index = addr_status_buffer + write_offset;
  uint16_t prev_index;

  // Wrap around case
  if (curr_index == addr_status_buffer) {
    prev_index = addr_status_buffer + buffer_len - 1;
  } else {
    prev_index = curr_index - 1;
  }

  uint16_t sb_val = EEPROM.readByte(prev_index) + 1;

  EEPROM.updateByte(curr_index, sb_val);
}

void settingsInit() {
  debugPrintf(F("Initialize Settings"));

  debugPrintf(F("\tVAR_SIZE: %02X"), var_size);
  debugPrintf(F("\tBUFFER_LEN: %02X"), buffer_len);
  debugPrintf(F("\tSTATUS_BUFFER_ADDRESS: %04X"), addr_status_buffer);

  _settingsLoad();
}

uint8_t settingsGetKeycode(uint8_t reg) {
  if (reg < REGISTER_CCW || reg > REGISTER_BTN) {
    return 0;
  }

  return settings[reg] & MASK_KEY;
}

uint8_t settingsGetModifiers(uint8_t reg) {
  if (reg < REGISTER_CCW || reg > REGISTER_BTN) {
    return 0;
  }

  return (settings[reg] & MASK_MOD) >> 8;
}

void settingsSetKeycode(uint8_t reg, uint8_t keycode) {
  if (reg < REGISTER_CCW || reg > REGISTER_BTN) {
    return 0;
  }

  settings[reg] &= 0xFF00; // clear out old values
  settings[reg] |= keycode;

  _settingsSave();
}

void settingsSetModifiers(uint8_t reg, uint8_t modifiers) {
  if (reg < REGISTER_CCW || reg > REGISTER_BTN) {
    return 0;
  }

  settings[reg] &= 0x00FF; // clear out old values
  settings[reg] |= (modifiers << 8);

  _settingsSave();
}
