#ifndef CLOCK_FACE_H
#define CLOCK_FACE_H

const PROGMEM uint8_t FACE_INNER_HOURLY_BRIGHTNESS[] = {
  0, 0, 0, 0, 0, 0,
  255, 255, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0,
  255, 255, 0, 0, 0, 0
};

const PROGMEM uint8_t FACE_OUTER_HOURLY_BRIGHTNESS[] = {
  255, 255, 255, 255, 255, 255,
  0, 0, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255,
  0, 0, 255, 255, 255, 255
};

#endif
