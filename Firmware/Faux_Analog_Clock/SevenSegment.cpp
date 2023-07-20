#include "Arduino.h"
#include "SevenSegment.h"

void writeSevenSegmentDisplay(FrameBufferView *frameBuffer, char value, uint8_t brightness) {
  uint8_t tableIndex = static_cast<uint8_t>(value & 0x7f);
  uint8_t segments = pgm_read_byte(SEVEN_SEGMENT_ASCII_TABLE + tableIndex);
  for (uint8_t i = 0; i < 7; ++i) {
    frameBuffer->setValue(i, ((segments & 0x1) > 0) ? brightness : 0);
    segments >>= 1;
  }
}
