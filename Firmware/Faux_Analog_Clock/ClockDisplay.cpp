#include "Arduino.h"
#include "FrameBufferView.h"
#include "ClockDisplay.h"

#define NOP __asm__ __volatile__ ("nop\n\t")

const uint8_t CLOCK_DISPLAY_PORTB_MASK = 0b00001111;
const uint8_t CLOCK_DISPLAY_PORTC_MASK = 0b00000011;

const uint8_t PIN_MASKS[CLOCK_DISPLAY_PIN_COUNT] = {
  0b00000001,
  0b00000010,
  0b00000100,
  0b00001000,
  0b00010000,
  0b00100000,
  0b01000000,
  0b10000000,
  0b00000001,
  0b00000010,
  0b00000100,
  0b00001000,
  0b00000001,
  0b00000010
};

ClockDisplay::ClockDisplay() {
  for (uint8_t i = 0; i < CLOCK_DISPLAY_LED_COUNT; ++i) {
    frameBuffer[i] = 0;
  }
}

void ClockDisplay::begin() {
  DDRD = 0;
  PORTD = 0;
  DDRB &= ~CLOCK_DISPLAY_PORTB_MASK;
  PORTB &= ~CLOCK_DISPLAY_PORTB_MASK;
  DDRC &= ~CLOCK_DISPLAY_PORTC_MASK;
  PORTC &= ~CLOCK_DISPLAY_PORTC_MASK;
}


void ClockDisplay::display() {
  uint8_t ledIndex = 0;
  for (uint8_t pos = 0; pos < CLOCK_DISPLAY_PIN_COUNT; ++pos) {
    // Set positive Charlieplex line
    uint8_t posMask = PIN_MASKS[pos];
    if (pos < 8) {
      DDRD = posMask;
      PORTD = posMask;
    } else if (pos < 12) {
      DDRB |= posMask;
      PORTB |= posMask;
    } else {
      DDRC |= posMask;
      PORTC |= posMask;
    }

    for (uint8_t neg = 0; neg < CLOCK_DISPLAY_PIN_COUNT; ++neg) {
      // There's obviously no LED that has both leads connected to the same line, so skip this case
      if (pos != neg) {
        uint8_t ledValue = frameBuffer[ledIndex];

        // No need to toggle any lines if the LED is off (this does change the timing a bit, but it's fine)
        if (ledValue > 0) {
          // Set negative Charlieplex line (output should already be low)
          uint8_t negMask = PIN_MASKS[neg];
          if (neg < 8) {
            DDRD |= negMask;
          } else if (neg < 12) {
            DDRB |= negMask;
          } else {
            DDRC |= negMask;
          }
        
          // On Delay
          timedWait(ledValue);

          // Clear negative Charlieplex line
          negMask = ~negMask;
          if (neg < 8) {
            DDRD &= negMask;
          } else if (neg < 12) {
            DDRB &= negMask;
          } else {
            DDRC &= negMask;
          }

          // Off delay
          if (ledValue < 255) {
            timedWait(255 - ledValue);
          }
        }

        // Move to the next LED
        ++ledIndex;
      }
    }

    // Clear positive Charlieplex line
    posMask = ~posMask;
    if (pos < 8) {
      DDRD &= posMask;
      PORTD &= posMask;
    } else if (pos < 12) {
      DDRB &= posMask;
      PORTB &= posMask;
    } else {
      DDRC &= posMask;
      PORTC &= posMask;
    }
  }
}

void ClockDisplay::setLEDValue(uint8_t index, uint8_t value) {
  if (index < CLOCK_DISPLAY_LED_COUNT) {
    frameBuffer[index] = value;
  }
}

void ClockDisplay::setLEDValues(uint8_t *source, uint8_t destIndex, uint8_t count) {
  uint8_t realCount = destIndex >= CLOCK_DISPLAY_LED_COUNT ? 0 : min(CLOCK_DISPLAY_LED_COUNT - destIndex, count);
  if (realCount > 0) {
    memcpy(frameBuffer + destIndex, source, realCount);
  }
}

void ClockDisplay::setAllLEDValues(uint8_t value) {
  memset(frameBuffer, value, CLOCK_DISPLAY_LED_COUNT);
}

FrameBufferView *ClockDisplay::newFrameBufferView(uint8_t startIndex, uint8_t count) {
  uint8_t realStartIndex = min(startIndex, CLOCK_DISPLAY_LED_COUNT - 1);
  uint8_t realCount = min(count, CLOCK_DISPLAY_LED_COUNT - realStartIndex);
  return new FrameBufferView(frameBuffer + realStartIndex, realCount);
}


void ClockDisplay::timedWait(uint8_t timeFrame) {
  // TODO: Tweak this
  // 15 ms scan at a delay length of 1 NOP
  // 18 ms scan at a delay length of 2 NOPs
  // 21 ms scan at a delay length of 3 NOPs
  // Basically 15 ms + 3 ms * (NOPs - 1)
  for (uint8_t i = 0; i < timeFrame; ++i) {
    NOP; NOP; NOP;
  }
}
